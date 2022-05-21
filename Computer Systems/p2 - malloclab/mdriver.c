/*
 * mdriver.c - CS:APP Malloc Lab Driver
 * 
 * Uses a collection of trace files to tests a malloc/free/realloc
 * implementation in mm.c.
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

#include "mm.h"
#include "memlib.h"
#include "fsecs.h"
#include "config.h"

/**********************
 * Constants and macros
 **********************/

/* Misc */
#define MAXLINE     1024 /* max string size */
#define HDRLINES       4 /* number of header lines in a trace file */
#define LINENUM(i) (i+5) /* cnvt trace request nums to linenums (origin 1) */

/* Returns true if p is ALIGNMENT-byte aligned */
static bool
IS_ALIGNED(void *p) {
    return ((uintptr_t) p) % ALIGNMENT == 0;
}

/****************************** 
 * The key compound data types 
 *****************************/

/* Records the extent of each block's payload */
typedef struct range_t {
    char *lo;              /* low payload address */
    char *hi;              /* high payload address */
    struct range_t *next;  /* next list element */
} range_t;

/* Characterizes a single trace operation (allocator request) */
typedef struct {
    enum {ALLOC, FREE, REALLOC} type; /* type of request */
    int index;                        /* index for free() to use later */
    int size;                         /* byte size of alloc/realloc request */
} traceop_t;

/* Holds the information for one trace file*/
typedef struct {
    int sugg_heapsize;   /* suggested heap size (unused) */
    int num_ids;         /* number of alloc/realloc ids */
    int num_ops;         /* number of distinct requests */
    int weight;          /* weight for this trace (unused) */
    double multiplier;   /* multiply sizes by this amount */
    traceop_t *ops;      /* array of requests */
    char **blocks;       /* array of ptrs returned by malloc/realloc... */
    size_t *block_sizes; /* ... and a corresponding array of payload sizes */
} trace_t;

/* Summarizes the important stats for some malloc function on some trace */
typedef struct {
    /* defined for both libc malloc and student malloc package (mm.c) */
    double ops;      /* number of ops (malloc/free/realloc) in the trace */
    int valid;       /* was the trace processed correctly by the allocator? */
    double secs;     /* number of secs needed to run the trace */

    /* defined only for the student malloc package */
    double util;     /* space utilization for this trace (always 0 for libc) */

    /* Note: secs and util are only defined if valid is true */
} stats_t; 

/********************
 * Global variables
 *******************/
int verbose = 0;        /* global flag for verbose output */
static int errors = 0;  /* number of errs found when running student malloc */

/* Directory where default tracefiles are found */
static char tracedir[MAXLINE] = TRACEDIR;

/* The filenames of the default tracefiles */
static char *default_tracefiles[] = {  
    DEFAULT_TRACEFILES, NULL
};


/********************* 
 * Function prototypes 
 *********************/

/* these functions manipulate range lists */
static int add_range(range_t **ranges, char *lo, int size, 
                     int tracenum, int opnum);
static __thread int check_heap_bounds;  /* if off, do not check if ranges are within heap bounds */
static void remove_range(range_t **ranges, char *lo);
static void clear_ranges(range_t **ranges);

/* These functions read, allocate, and free storage for traces */
static trace_t *read_trace(char *tracedir, char *filename, int verbose);
static void free_trace(trace_t *trace);

/* Routines for evaluating the correctness and speed of libc malloc */
static int eval_libc_valid(trace_t *trace, int tracenum);
static void eval_libc_speed(void *ptr);

/* Routines for evaluating correctnes, space utilization, and speed 
   of the student's malloc package in mm.c */
static int reset_heap(int tracenum);
static int eval_mm_valid(trace_t *trace, int tracenum, range_t **ranges);
static int eval_mm_valid_inner(trace_t *trace, int tracenum, range_t **ranges);
struct single_run_args_for_valid {
    char * tracefilename;
    int tracenum;
    pthread_barrier_t *go;
};
struct thread_run_result {
    double secs;            // number of seconds used
    int heapsize;      // size to which memlib heap grew
};
static void * eval_mm_valid_single(void *);
static int eval_mm_util(trace_t *trace, int tracenum, range_t **ranges);
static void eval_mm_speed(void *ptr);
static void eval_mm_speed_inner(void *ptr);
static void * eval_mm_speed_single(void *_args);

/* Various helper routines */
static void printresults(int n, char ** tracefiles, stats_t *stats);
static void printresults_as_json(FILE *json, int n, char ** tracefiles, stats_t *stats);
static void usage(void);
static void unix_error(char *msg);
static void malloc_error(int tracenum, int opnum, char *msg);
static void app_error(char *msg);
static FILE * open_jsonfile(const char * filename_mask);

static int 
max(int a, int b)
{
    return a < b ? b : a;
}

/**************
 * Main routine
 **************/
int main(int argc, char **argv)
{
    int i;
    char c;
    char **tracefiles = NULL;  /* null-terminated array of trace file names */
    int num_tracefiles = 0;    /* the number of traces in that array */
    trace_t *trace = NULL;     /* stores a single trace file in memory */
    range_t *ranges = NULL;    /* keeps track of block extents for one trace */
    stats_t *libc_stats = NULL;/* libc stats for each trace */
    stats_t *mm_stats = NULL;  /* mm (i.e. student) stats for each trace */

    int team_check = 1;  /* If set, check team structure (reset by -a) */
    int run_libc = 0;    /* If set, run libc malloc (set by -l) */
    int nthreads = 0;    /* If set to > 0, number of threads for multi-threaded testing. */
    int autograder = 0;  /* If set, emit summary info for autograder (-g) */
    int use_mmap = 0;    /* If set, have memlib use mmap() instead malloc() */
    int vary_size = 0;   /* If set, run each trace multiple times with varied sizes */

    /* temporaries used to compute the performance index */
    double secs, ops, util, avg_mm_util, avg_mm_throughput = 0, p1, p2, perfindex;
    int numcorrect;
    
    /* 
     * Read and interpret the command line arguments 
     */
    while ((c = getopt(argc, argv, "nf:t:hvVgalm:s")) != EOF) {
        switch (c) {
        case 's':
            vary_size = 1;
            break;
        case 'n':
            use_mmap = 1;
            break;
        case 'g': /* Generate summary info for the autograder */
            autograder = 1;
            break;
        case 'f': /* Use one specific trace file only (relative to curr dir) */
            num_tracefiles = 1;
            if ((tracefiles = realloc(tracefiles, 2*sizeof(char *))) == NULL)
                unix_error("ERROR: realloc failed in main");
            strcpy(tracedir, "./"); 
            tracefiles[0] = strdup(optarg);
            tracefiles[1] = NULL;
            break;
        case 't': /* Directory where the traces are located */
            if (num_tracefiles == 1) /* ignore if -f already encountered */
                break;
            strcpy(tracedir, optarg);
            if (tracedir[strlen(tracedir)-1] != '/') 
                strcat(tracedir, "/"); /* path always ends with "/" */
            break;
        case 'a': /* Don't check team structure */
            team_check = 0;
            break;
        case 'l': /* Run libc malloc */
            run_libc = 1;
            break;
        case 'v': /* Print per-trace performance breakdown */
            verbose = 1;
            break;
        case 'V': /* Be more verbose than -v */
            verbose = 2;
            break;
        case 'm': /* Include multi-threaded testing */
            nthreads = atoi(optarg);
            break;
        case 'h': /* Print this message */
            usage();
            exit(0);
        default:
            usage();
            exit(1);
        }
    }
        
    /* 
     * Check and print team info 
     */
    if (team_check) {
        /* Students must fill in their team information */
        if (!strcmp(team.teamname, "")) {
            printf("ERROR: Please provide the information about your team in mm.c.\n");
            exit(1);
        } else
            printf("Team Name:%s\n", team.teamname);
        if ((*team.name1 == '\0') || (*team.id1 == '\0')) {
            printf("ERROR.  You must fill in all team member 1 fields!\n");
            exit(1);
        } 
        else
            printf("Member 1 :%s:%s\n", team.name1, team.id1);

        if (((*team.name2 != '\0') && (*team.id2 == '\0')) ||
            ((*team.name2 == '\0') && (*team.id2 != '\0'))) { 
            printf("ERROR.  You must fill in all or none of the team member 2 ID fields!\n");
            exit(1);
        }
        else if (*team.name2 != '\0')
            printf("Member 2 :%s:%s\n", team.name2, team.id2);
    }

    /* 
     * If no -f command line arg, then use the entire set of tracefiles 
     * defined in default_traces[]
     */
    if (tracefiles == NULL) {
        tracefiles = default_tracefiles;
        num_tracefiles = sizeof(default_tracefiles) / sizeof(char *) - 1;
        printf("Using default tracefiles in %s\n", tracedir);
    }

    /* Initialize the timing package */
    init_fsecs();

    /*
     * Optionally run and evaluate the libc malloc package 
     */
    if (run_libc) {
        if (verbose > 1)
            printf("\nTesting libc malloc\n");
        
        /* Allocate libc stats array, with one stats_t struct per tracefile */
        libc_stats = (stats_t *)calloc(num_tracefiles, sizeof(stats_t));
        if (libc_stats == NULL)
            unix_error("libc_stats calloc in main failed");
        
        /* Evaluate the libc malloc package using the K-best scheme */
        for (i=0; i < num_tracefiles; i++) {
            trace = read_trace(tracedir, tracefiles[i], verbose > 1);
            libc_stats[i].ops = trace->num_ops;
            if (verbose > 1)
                printf("Checking libc malloc for correctness, ");
            libc_stats[i].valid = eval_libc_valid(trace, i);
            if (libc_stats[i].valid) {
                if (verbose > 1)
                    printf("and performance.\n");
                libc_stats[i].secs = fsecs(eval_libc_speed, trace);
            }
            free_trace(trace);
        }

        /* Display the libc results in a compact table */
        if (verbose) {
            printf("\nResults for libc malloc:\n");
            printresults(num_tracefiles, tracefiles, libc_stats);
        }
    }

    /*
     * Always run and evaluate the student's mm package
     */
    if (verbose > 1)
        printf("\nTesting mm malloc\n");

    /* Allocate the mm stats array, with two stats_t struct per tracefile */
    mm_stats = (stats_t *)calloc((nthreads > 0 ? 2 : 1) * num_tracefiles, sizeof(stats_t));
    if (mm_stats == NULL)
        unix_error("mm_stats calloc in main failed");
    
    /* Initialize the simulated memory system in memlib.c */
    mem_init(use_mmap); 

    int max_total_size = 0;

    double * size_multipliers;
    int n_multipliers;
    double one[] = { 1.0 };
    double many[] = { .75, 1.0, 1.25 };
    if (vary_size) {
        size_multipliers = many;
        n_multipliers = sizeof(many)/sizeof(many[0]);
    } else {
        size_multipliers = one;
        n_multipliers = 1;
    }

    /* Evaluate student's mm malloc package using the K-best scheme */
    for (i=0; i < num_tracefiles; i++) {
        trace = read_trace(tracedir, tracefiles[i], verbose > 1);
        mm_stats[i].ops = trace->num_ops;
        mm_stats[i].valid = 1;
        mm_stats[i].util = 0.0;
        mm_stats[i].secs = 0.0;
        for (int mi = 0; mi < n_multipliers; mi++) {
            trace->multiplier = size_multipliers[mi];
            if (verbose > 1 && vary_size)
                printf("Using trace multiplier: %f\n", size_multipliers[mi]);

            if (verbose > 1)
                printf("Checking mm_malloc for correctness, ");

            check_heap_bounds = 1;
            int thisrunvalid = eval_mm_valid(trace, i, &ranges);
            if (!thisrunvalid)
                mm_stats[i].valid = 0;

            if (mm_stats[i].valid) {
                if (verbose > 1)
                    printf("efficiency, ");

                int hwm = eval_mm_util(trace, i, &ranges);
                if (size_multipliers[mi] == 1.0)    // record max high water mark
                    max_total_size = hwm;
                mm_stats[i].util += ((double)hwm / (double)mem_heapsize());
                if (verbose > 1)
                    printf("and performance.\n");
                mm_stats[i].secs += fsecs(eval_mm_speed, trace);
            }
        }
        mm_stats[i].util /= n_multipliers;
        mm_stats[i].secs /= n_multipliers;

        /* Test multithreaded behavior */
        if (nthreads) {
            if (verbose > 1)
                printf("Checking multithreaded mm_malloc for correctness\n");
            stats_t * ms = &mm_stats[i+num_tracefiles];
            ms->ops = trace->num_ops * nthreads;

            struct single_run_args_for_valid args[nthreads];
            pthread_t threads[nthreads];
            pthread_barrier_t go;
            if (pthread_barrier_init(&go, NULL, nthreads)) {
                perror("pthread_barrier_init");
                abort();
            }
            reset_heap(i);

            for (int j = 0; j < nthreads; j++) {
                args[j].go = &go;
                args[j].tracefilename = tracefiles[i];
                args[j].tracenum = i;
                if (pthread_create(threads + j, NULL, eval_mm_valid_single, args + j))
                    perror("pthread_create"), exit(-1);
            }

            ms->valid = 1;
            for (int j = 0; j < nthreads; j++) {
                uintptr_t this_run_valid;
                if (pthread_join(threads[j], (void **)&this_run_valid))
                    perror("pthread_join"), exit(-1);

                if (!this_run_valid)
                    ms->valid = 0;
            }
            if (verbose > 1)
                printf("Result appears to be valid.\n");

            if (!ms->valid) {
                printf("Result is not valid, skipping further multithreads tests.\n");
            } else {
                // we know max_total_size, the total amount of heap memory may vary.
                // benchmark it a few times and take the average of utilization and speed.
                const int REPEATS = 2;
                long heap_size_avg = 0;
                double runtime_avg = 0.0;
                for (int k = 0; k < REPEATS; k++) {
                    reset_heap(i);

                    for (int j = 0; j < nthreads; j++) {
                        args[j].go = &go;
                        args[j].tracefilename = tracefiles[i];
                        args[j].tracenum = i;
                        if (pthread_create(threads + j, NULL, eval_mm_speed_single, args + j))
                            perror("pthread_create"), exit(-1);
                    }

                    for (int j = 0; j < nthreads; j++) {
                        struct thread_run_result *r;
                        if (pthread_join(threads[j], (void **) &r))
                            perror("pthread_join"), exit(-1);
                        if (r) {
                            runtime_avg += r->secs;
                            heap_size_avg += r->heapsize;
                            free(r);
                        }
                    }
                }
                runtime_avg /= REPEATS;
                heap_size_avg /= REPEATS;
                ms->util = ((double)nthreads * max_total_size) / heap_size_avg;
                ms->secs = runtime_avg;
            }
        }
        free_trace(trace);
    }

    /* Display the mm results in a compact table */
    if (verbose) {
        printf("\nResults for mm malloc:\n");
        printresults(num_tracefiles, tracefiles, mm_stats);
        printf("\n");
    }

    if (nthreads && verbose) {
        printf("\nResults for multi-threaded mm malloc:\n");
        printresults(num_tracefiles, tracefiles, mm_stats+num_tracefiles);
        printf("\n");
    }

    /* 
     * Accumulate the aggregate statistics for the student's mm package 
     */
    secs = 0;
    ops = 0;
    util = 0;
    numcorrect = 0;
    for (i=0; i < num_tracefiles; i++) {
        secs += mm_stats[i].secs;
        ops += mm_stats[i].ops;
        util += mm_stats[i].util;
        if (mm_stats[i].valid)
            numcorrect++;
    }
    avg_mm_util = util/num_tracefiles;

    /* 
     * Compute and print the performance index 
     */
    if (errors == 0) {
        avg_mm_throughput = ops/secs;

        p1 = UTIL_WEIGHT * avg_mm_util;
        if (avg_mm_throughput > AVG_LIBC_THRUPUT) {
            p2 = (double)(1.0 - UTIL_WEIGHT);
        } 
        else {
            p2 = ((double) (1.0 - UTIL_WEIGHT)) * 
                (avg_mm_throughput/AVG_LIBC_THRUPUT);
        }
        
        perfindex = (p1 + p2)*100.0;
        printf("Perf index = %.0f (util) + %.0f (thru) = %.0f/100\n",
               p1*100, 
               p2*100, 
               perfindex);
        
    }
    else { /* There were errors */
        perfindex = 0.0;
        printf("Terminated with %d errors\n", errors);
    }

    if (autograder) {
        printf("correct:%d\n", numcorrect);
        printf("perfidx:%.0f\n", perfindex);
    }

    /* Write results to JSON file for submission */
    FILE * json = open_jsonfile("results.%d.json");
    fprintf(json, "{");
    fprintf(json, " \"version\": \"1.1\",\n");
    fprintf(json, " \"varysize\": %d,\n", vary_size);
    fprintf(json, " \"nthreads\": %d,\n", nthreads);
#ifdef THREAD_SAFE
    fprintf(json, " \"THREAD_SAFE\": true,\n");
#else
    fprintf(json, " \"THREAD_SAFE\": false,\n");
#endif
    fprintf(json, " \"results\": ");
    printresults_as_json(json, num_tracefiles, tracefiles, mm_stats);
    if (errors == 0) {
        fprintf(json, ", \"perfindex\" : "
                "{ \"avg_mm_util\": %f, \"avg_mm_throughput\" : %f, \"perfindex\": %f, \"AVG_LIBC_THRUPUT\": %f }\n", 
                avg_mm_util, avg_mm_throughput, perfindex, AVG_LIBC_THRUPUT);
    }
    if (nthreads > 0) {
        fprintf(json, ", \"mtresults\": ");
        printresults_as_json(json, num_tracefiles, tracefiles, mm_stats+num_tracefiles);
    }
    fprintf(json, "}");
    fclose(json);

    exit(0);
}

static FILE *
open_jsonfile(const char * filename_mask)
{
    char jsonfilename[80];
    snprintf(jsonfilename, sizeof jsonfilename, filename_mask, getpid());
    printf("Writing results to %s for submission to the scoreboard\n", jsonfilename);
    return fopen(jsonfilename, "w");
}


/*****************************************************************
 * The following routines manipulate the range list, which keeps 
 * track of the extent of every allocated block payload. We use the 
 * range list to detect any overlapping allocated blocks.
 ****************************************************************/

/*
 * add_range - As directed by request opnum in trace tracenum,
 *     we've just called the student's mm_malloc to allocate a block of 
 *     size bytes at addr lo. After checking the block for correctness,
 *     we create a range struct for this block and add it to the range list. 
 */
static int add_range(range_t **ranges, char *lo, int size, 
                     int tracenum, int opnum)
{
    char *hi = lo + size - 1;
    range_t *p;
    char msg[MAXLINE];

    assert(size > 0);

    /* Payload addresses must be ALIGNMENT-byte aligned */
    if (!IS_ALIGNED(lo)) {
        sprintf(msg, "Payload address (%p) not aligned to %d bytes", 
                lo, ALIGNMENT);
        malloc_error(tracenum, opnum, msg);
        return 0;
    }

    /* The payload must lie within the extent of the heap */
    if (check_heap_bounds && (
        (lo < (char *)mem_heap_lo()) || (lo > (char *)mem_heap_hi()) || 
        (hi < (char *)mem_heap_lo()) || (hi > (char *)mem_heap_hi()))) {
        sprintf(msg, "Payload (%p:%p) lies outside heap (%p:%p)",
                lo, hi, mem_heap_lo(), mem_heap_hi());
        malloc_error(tracenum, opnum, msg);
        return 0;
    }

    /* The payload must not overlap any other payloads */
    for (p = *ranges;  p != NULL;  p = p->next) {
        if ((lo >= p->lo && lo <= p->hi) ||
            (hi >= p->lo && hi <= p->hi)) {
            sprintf(msg, "Payload (%p:%p) overlaps another payload (%p:%p)\n",
                    lo, hi, p->lo, p->hi);
            malloc_error(tracenum, opnum, msg);
            return 0;
        }
    }

    /* 
     * Everything looks OK, so remember the extent of this block 
     * by creating a range struct and adding it the range list.
     */
    if ((p = (range_t *)malloc(sizeof(range_t))) == NULL)
        unix_error("malloc error in add_range");
    p->next = *ranges;
    p->lo = lo;
    p->hi = hi;
    *ranges = p;
    return 1;
}

/* 
 * remove_range - Free the range record of block whose payload starts at lo 
 */
static void remove_range(range_t **ranges, char *lo)
{
    range_t *p;
    range_t **prevpp = ranges;

    for (p = *ranges;  p != NULL; p = p->next) {
        if (p->lo == lo) {
            *prevpp = p->next;
            free(p);
            break;
        }
        prevpp = &(p->next);
    }
}

/*
 * clear_ranges - free all of the range records for a trace 
 */
static void clear_ranges(range_t **ranges)
{
    range_t *p;
    range_t *pnext;

    for (p = *ranges;  p != NULL;  p = pnext) {
        pnext = p->next;
        free(p);
    }
    *ranges = NULL;
}


/**********************************************
 * The following routines manipulate tracefiles
 *********************************************/

/*
 * read_trace - read a trace file and store it in memory
 */
static trace_t *read_trace(char *tracedir, char *filename, int verbose)
{
    char msg[MAXLINE + 100];
    FILE *tracefile;
    trace_t *trace;
    char type[MAXLINE];
    char path[MAXLINE];
    unsigned index, size;
    unsigned max_index = 0;
    unsigned op_index;

    if (verbose)
        printf("Reading tracefile: %s\n", filename);

    /* Allocate the trace record */
    if ((trace = (trace_t *) malloc(sizeof(trace_t))) == NULL)
        unix_error("malloc 1 failed in read_trance");
        
    trace->multiplier = 1.0;

    /* Read the trace file header */
    strcpy(path, tracedir);
    strcat(path, filename);
    if ((tracefile = fopen(path, "r")) == NULL) {
        snprintf(msg, sizeof msg, "Could not open %s in read_trace", path);
        unix_error(msg);
    }
    int rc;
    rc = fscanf(tracefile, "%d", &(trace->sugg_heapsize)); /* not used */
    if (rc != 1) abort();
    rc = fscanf(tracefile, "%d", &(trace->num_ids));     
    if (rc != 1) abort();
    rc = fscanf(tracefile, "%d", &(trace->num_ops));     
    if (rc != 1) abort();
    rc = fscanf(tracefile, "%d", &(trace->weight));        /* not used */
    if (rc != 1) abort();
    
    /* We'll store each request line in the trace in this array */
    if ((trace->ops = 
         (traceop_t *)malloc(trace->num_ops * sizeof(traceop_t))) == NULL)
        unix_error("malloc 2 failed in read_trace");

    /* We'll keep an array of pointers to the allocated blocks here... */
    if ((trace->blocks = 
         (char **)malloc(trace->num_ids * sizeof(char *))) == NULL)
        unix_error("malloc 3 failed in read_trace");

    /* ... along with the corresponding byte sizes of each block */
    if ((trace->block_sizes = 
         (size_t *)malloc(trace->num_ids * sizeof(size_t))) == NULL)
        unix_error("malloc 4 failed in read_trace");
    
    /* read every request line in the trace file */
    index = 0;
    op_index = 0;
    while (fscanf(tracefile, "%s", type) != EOF) {
        switch(type[0]) {
        case 'a':
            rc = fscanf(tracefile, "%u %u", &index, &size);
            assert (rc == 2);
            trace->ops[op_index].type = ALLOC;
            trace->ops[op_index].index = index;
            trace->ops[op_index].size = size;
            max_index = (index > max_index) ? index : max_index;
            break;
        case 'r':
            rc = fscanf(tracefile, "%u %u", &index, &size);
            assert (rc == 2);
            trace->ops[op_index].type = REALLOC;
            trace->ops[op_index].index = index;
            trace->ops[op_index].size = size;
            max_index = (index > max_index) ? index : max_index;
            break;
        case 'f':
            rc = fscanf(tracefile, "%ud", &index);
            assert (rc == 1);
            trace->ops[op_index].type = FREE;
            trace->ops[op_index].index = index;
            break;
        default:
            printf("Bogus type character (%c) in tracefile %s\n", 
                   type[0], path);
            exit(1);
        }
        op_index++;
        
    }
    fclose(tracefile);
    assert(max_index == trace->num_ids - 1);
    assert(trace->num_ops == op_index);
    
    return trace;
}

/*
 * free_trace - Free the trace record and the three arrays it points
 *              to, all of which were allocated in read_trace().
 */
void free_trace(trace_t *trace)
{
    free(trace->ops);         /* free the three arrays... */
    free(trace->blocks);      
    free(trace->block_sizes);
    free(trace);              /* and the trace record itself... */
}

/**********************************************************************
 * The following functions evaluate the correctness, space utilization,
 * and throughput of the libc and mm malloc packages.
 **********************************************************************/

static int
reset_heap(int tracenum)
{
    /* Reset the heap and free any records in the range list */
    mem_reset_brk();

    /* Call the mm package's init function */
    if (mm_init() < 0) {
        malloc_error(tracenum, 0, "mm_init failed.");
        return 0;
    }
    return 1;
}

/*
 * eval_mm_valid - Check the mm malloc package for correctness
 */
static int eval_mm_valid(trace_t *trace, int tracenum, range_t **ranges)
{
    if (!reset_heap(tracenum))
        return 0;

    return eval_mm_valid_inner(trace, tracenum, ranges);
}

static int eval_mm_valid_inner(trace_t *trace, int tracenum, range_t **ranges)
{
    int i, j;
    int index;
    int size;
    int oldsize;
    char *newp;
    char *oldp;
    char *p;
    
    /* Reset the heap and free any records in the range list */
    clear_ranges(ranges);

    /* Interpret each operation in the trace in order */
    for (i = 0;  i < trace->num_ops;  i++) {
        index = trace->ops[i].index;
        int rsize = (int) (trace->multiplier * trace->ops[i].size);
        size = max(0, (int)rsize);

        switch (trace->ops[i].type) {

        case ALLOC: /* mm_malloc */

            /* Call the student's malloc */
            if ((p = mm_malloc(size)) == NULL) {
                malloc_error(tracenum, i, "mm_malloc failed.");
                return 0;
            }
            
            /* 
             * Test the range of the new block for correctness and add it 
             * to the range list if OK. The block must be  be aligned properly,
             * and must not overlap any currently allocated block. 
             */ 
            if (add_range(ranges, p, size, tracenum, i) == 0)
                return 0;
            
            /* ADDED: cgw
             * fill range with low byte of index.  This will be used later
             * if we realloc the block and wish to make sure that the old
             * data was copied to the new block
             */
            memset(p, index & 0xFF, size);

            /* Remember region */
            trace->blocks[index] = p;
            trace->block_sizes[index] = size;
            break;

        case REALLOC: /* mm_realloc */
            
            /* Call the student's realloc */
            oldp = trace->blocks[index];
            if ((newp = mm_realloc(oldp, size)) == NULL) {
                malloc_error(tracenum, i, "mm_realloc failed.");
                return 0;
            }
            
            /* Remove the old region from the range list */
            remove_range(ranges, oldp);
            
            /* Check new block for correctness and add it to range list */
            if (add_range(ranges, newp, size, tracenum, i) == 0)
                return 0;
            
            /* ADDED: cgw
             * Make sure that the new block contains the data from the old 
             * block and then fill in the new block with the low order byte
             * of the new index
             */
            oldsize = trace->block_sizes[index];
            if (size < oldsize) oldsize = size;
            for (j = 0; j < oldsize; j++) {
              if (newp[j] != (index & 0xFF)) {
                malloc_error(tracenum, i, "mm_realloc did not preserve the "
                             "data from old block");
                return 0;
              }
            }
            memset(newp, index & 0xFF, size);

            /* Remember region */
            trace->blocks[index] = newp;
            trace->block_sizes[index] = size;
            break;

        case FREE: /* mm_free */
            
            /* Remove region from list and call student's free function */
            p = trace->blocks[index];
            remove_range(ranges, p);
            mm_free(p);
            break;

        default:
            app_error("Nonexistent request type in eval_mm_valid");
        }

    }

    /* As far as we know, this is a valid malloc package */
    return 1;
}

static void *
eval_mm_valid_single(void *_args)
{
    struct single_run_args_for_valid * args = _args;

    /* read our own copy of this trace */
    trace_t * trace = read_trace(tracedir, args->tracefilename, 0);

    // let threads start at approximately the same moment to increase chance
    // of concurrency-related failures if proper synchronization is not used.
    if (pthread_barrier_wait(args->go) == PTHREAD_BARRIER_SERIAL_THREAD) {
        ;
    }
    range_t *ranges = NULL;
    check_heap_bounds = 0;
    intptr_t isvalid = eval_mm_valid_inner(trace, args->tracenum, &ranges);
    assert (sizeof(int) <= sizeof(void*));
    clear_ranges(&ranges);
    free_trace(trace);
    return (void *) isvalid;
}

/* 
 * eval_mm_util - Evaluate the space utilization of the student's package
 *   The idea is to remember the high water mark "hwm" of the heap for 
 *   an optimal allocator, i.e., no gaps and no internal fragmentation.
 *   Utilization is the ratio hwm/heapsize, where heapsize is the 
 *   size of the heap in bytes after running the student's malloc 
 *   package on the trace. Note that our implementation of mem_sbrk() 
 *   doesn't allow the students to decrement the brk pointer, so brk
 *   is always the high water mark of the heap. 
 *   
 *   Changed to return max_total_size
 */
static int eval_mm_util(trace_t *trace, int tracenum, range_t **ranges)
{   
    int i;
    int index;
    int size, newsize, oldsize;
    int max_total_size = 0;
    int total_size = 0;
    char *p;
    char *newp, *oldp;

    /* initialize the heap and the mm malloc package */
    mem_reset_brk();
    if (mm_init() < 0)
        app_error("mm_init failed in eval_mm_util");

    for (i = 0;  i < trace->num_ops;  i++) {
        switch (trace->ops[i].type) {

        case ALLOC: /* mm_alloc */
            index = trace->ops[i].index;
            size = max(0, (int)(trace->multiplier * trace->ops[i].size));

            if ((p = mm_malloc(size)) == NULL) 
                app_error("mm_malloc failed in eval_mm_util");
            
            /* Remember region and size */
            trace->blocks[index] = p;
            trace->block_sizes[index] = size;
            
            /* Keep track of current total size
             * of all allocated blocks */
            total_size += size;
            
            /* Update statistics */
            max_total_size = (total_size > max_total_size) ?
                total_size : max_total_size;
            break;

        case REALLOC: /* mm_realloc */
            index = trace->ops[i].index;
            newsize = max(0, (int)(trace->multiplier * trace->ops[i].size));
            oldsize = trace->block_sizes[index];

            oldp = trace->blocks[index];
            if ((newp = mm_realloc(oldp,newsize)) == NULL)
                app_error("mm_realloc failed in eval_mm_util");

            /* Remember region and size */
            trace->blocks[index] = newp;
            trace->block_sizes[index] = newsize;
            
            /* Keep track of current total size
             * of all allocated blocks */
            total_size += (newsize - oldsize);
            
            /* Update statistics */
            max_total_size = (total_size > max_total_size) ?
                total_size : max_total_size;
            break;

        case FREE: /* mm_free */
            index = trace->ops[i].index;
            size = trace->block_sizes[index];
            p = trace->blocks[index];
            
            mm_free(p);
            
            /* Keep track of current total size
             * of all allocated blocks */
            total_size -= size;
            
            break;

        default:
            app_error("Nonexistent request type in eval_mm_util");

        }
    }

    return max_total_size;
}

static void *
eval_mm_speed_single(void *_args)
{
    struct single_run_args_for_valid * args = _args;
    struct timeval stv, etv;

    /* read our own copy of this trace */
    trace_t * trace = read_trace(tracedir, args->tracefilename, 0);

    // we start the clock here to avoid accounting for thread startup overhead
    pthread_barrier_wait(args->go);
    gettimeofday(&stv, NULL);
    eval_mm_speed_inner(trace);
    gettimeofday(&etv,NULL);
    free_trace(trace);
    if (pthread_barrier_wait(args->go) == PTHREAD_BARRIER_SERIAL_THREAD) {
        struct thread_run_result *r = malloc(sizeof (*r));
        r->secs = (etv.tv_sec - stv.tv_sec) + 1E-6*(etv.tv_usec-stv.tv_usec);
        r->heapsize = mem_heapsize();
        return r;
    } else
        return NULL;
}

/*
 * eval_mm_speed - This is the function that is used by fcyc()
 *    to measure the running time of the mm malloc package.
 */
static void eval_mm_speed(void *ptr)
{
    /* Reset the heap and initialize the mm package */
    mem_reset_brk();
    if (mm_init() < 0) 
        app_error("mm_init failed in eval_mm_speed");

    eval_mm_speed_inner(ptr);
}

static void eval_mm_speed_inner(void *ptr)
{
    int i, index, size, newsize;
    char *p, *newp, *oldp, *block;
    const trace_t *trace = (trace_t *)ptr;


    /* Interpret each trace request */
    for (i = 0;  i < trace->num_ops;  i++)
        switch (trace->ops[i].type) {

        case ALLOC: /* mm_malloc */
            index = trace->ops[i].index;
            size = max(0, (int)(trace->multiplier * trace->ops[i].size));
            if ((p = mm_malloc(size)) == NULL)
                app_error("mm_malloc error in eval_mm_speed");
            trace->blocks[index] = p;
            break;

        case REALLOC: /* mm_realloc */
            index = trace->ops[i].index;
            newsize = max(0, (int)(trace->multiplier * trace->ops[i].size));
            oldp = trace->blocks[index];
            if ((newp = mm_realloc(oldp,newsize)) == NULL)
                app_error("mm_realloc error in eval_mm_speed");
            trace->blocks[index] = newp;
            break;

        case FREE: /* mm_free */
            index = trace->ops[i].index;
            block = trace->blocks[index];
            mm_free(block);
            break;

        default:
            app_error("Nonexistent request type in eval_mm_valid");
        }
}

/*
 * eval_libc_valid - We run this function to make sure that the
 *    libc malloc can run to completion on the set of traces.
 *    We'll be conservative and terminate if any libc malloc call fails.
 *
 */
static int eval_libc_valid(trace_t *trace, int tracenum)
{
    int i, newsize, size;
    char *p, *newp, *oldp;

    for (i = 0;  i < trace->num_ops;  i++) {
        switch (trace->ops[i].type) {

        case ALLOC: /* malloc */
            size = max(0, (int)(trace->multiplier * trace->ops[i].size));
            if ((p = malloc(size)) == NULL) {
                malloc_error(tracenum, i, "libc malloc failed");
                unix_error("System message");
            }
            trace->blocks[trace->ops[i].index] = p;
            break;

        case REALLOC: /* realloc */
            newsize = max(0, (int)(trace->multiplier * trace->ops[i].size));
            oldp = trace->blocks[trace->ops[i].index];
            if ((newp = realloc(oldp, newsize)) == NULL) {
                malloc_error(tracenum, i, "libc realloc failed");
                unix_error("System message");
            }
            trace->blocks[trace->ops[i].index] = newp;
            break;
            
        case FREE: /* free */
            free(trace->blocks[trace->ops[i].index]);
            break;

        default:
            app_error("invalid operation type  in eval_libc_valid");
        }
    }

    return 1;
}

/* 
 * eval_libc_speed - This is the function that is used by fcyc() to
 *    measure the running time of the libc malloc package on the set
 *    of traces.
 */
static void eval_libc_speed(void *ptr)
{
    int i;
    int index, size, newsize;
    char *p, *newp, *oldp, *block;
    trace_t *trace = (trace_t *)ptr;

    for (i = 0;  i < trace->num_ops;  i++) {
        switch (trace->ops[i].type) {
        case ALLOC: /* malloc */
            index = trace->ops[i].index;
            size = max(0, (int)(trace->multiplier * trace->ops[i].size));
            if ((p = malloc(size)) == NULL)
                unix_error("malloc failed in eval_libc_speed");
            trace->blocks[index] = p;
            break;

        case REALLOC: /* realloc */
            index = trace->ops[i].index;
            newsize = max(0, (int)(trace->multiplier * trace->ops[i].size));
            oldp = trace->blocks[index];
            if ((newp = realloc(oldp, newsize)) == NULL)
                unix_error("realloc failed in eval_libc_speed\n");
            
            trace->blocks[index] = newp;
            break;
            
        case FREE: /* free */
            index = trace->ops[i].index;
            block = trace->blocks[index];
            free(block);
            break;
        }
    }
}

/*************************************
 * Some miscellaneous helper routines
 ************************************/


/*
 * printresults - prints a performance summary for some malloc package
 */
static void printresults(int n, char ** tracefiles, stats_t *stats) 
{
    int i;
    double secs = 0;
    double ops = 0;
    double util = 0;

    /* Print the individual results for each trace */
    printf("%5s%22s%5s%5s%8s%10s%6s\n", 
           "trace", " name", " valid", "util", "ops", "secs", "Kops");
    for (i=0; i < n; i++) {
        if (stats[i].valid) {
            printf("%2d%25s%5s%5.0f%%%8.0f%10.6f%6.0f\n", 
                   i,
                   tracefiles[i],
                   "yes",
                   stats[i].util*100.0,
                   stats[i].ops,
                   stats[i].secs,
                   (stats[i].ops/1e3)/stats[i].secs);
            secs += stats[i].secs;
            ops += stats[i].ops;
            util += stats[i].util;
        }
        else {
            printf("%2d%25s%5s%6s%8s%10s%6s\n", 
                   i,
                   tracefiles[i],
                   "no",
                   "-",
                   "-",
                   "-",
                   "-");
        }
    }

    /* Print the aggregate results for the set of traces */
    if (errors == 0) {
        printf("%12s                    %5.0f%%%8.0f%10.6f%6.0f\n", 
               "Total       ",
               (util/n)*100.0,
               ops, 
               secs,
               (ops/1e3)/secs);
    }
    else {
        printf("%12s                    %6s%8s%10s%6s\n", 
               "Total       ",
               "-", 
               "-", 
               "-", 
               "-");
    }

}

static void printresults_as_json(FILE *json, int n, char **tracefiles, stats_t *stats) 
{
    int i;
    double secs = 0;
    double ops = 0;
    double util = 0;

    /* Print the individual results for each trace */
    fprintf(json, "[\n");
    for (i=0; i < n; i++) {
        if (stats[i].valid) {
            fprintf(json, "{ \"%s\": \"%s\"\n", "trace", tracefiles[i]);
            fprintf(json, ", \"valid\": true\n");
            fprintf(json, ", \"%s\": %f\n", "util", stats[i].util*100.0);
            fprintf(json, ", \"%s\": %f\n", "ops", stats[i].ops);
            fprintf(json, ", \"%s\": %f\n", "secs", stats[i].secs);
            fprintf(json, ", \"%s\": %f\n", "Kops", (stats[i].ops/1e3)/stats[i].secs);
            fprintf(json, "}");

            secs += stats[i].secs;
            ops += stats[i].ops;
            util += stats[i].util;
        }
        else {
            fprintf(json, "{ \"%s\": \"%s\"\n", "trace", tracefiles[i]);
            fprintf(json, ", \"valid\": false\n");
            fprintf(json, "}");
        }
        if (i < n-1)
            fprintf(json, ",\n");
    }
    fprintf(json, "]\n");
}

/* 
 * app_error - Report an arbitrary application error
 */
void app_error(char *msg) 
{
    printf("%s\n", msg);
    exit(1);
}

/* 
 * unix_error - Report a Unix-style error
 */
void unix_error(char *msg) 
{
    printf("%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * malloc_error - Report an error returned by the mm_malloc package
 */
void malloc_error(int tracenum, int opnum, char *msg)
{
    errors++;
    printf("ERROR [trace %d, line %d]: %s\n", tracenum, LINENUM(opnum), msg);
}

/* 
 * usage - Explain the command line arguments
 */
static void usage(void) 
{
    fprintf(stderr, "Usage: mdriver [-shvVal] [-f <file>] [-m <t>] [-t <dir>]\n");
    fprintf(stderr, "Options\n");
    fprintf(stderr, "\t-a         Don't check the team structure.\n");
    fprintf(stderr, "\t-f <file>  Use <file> as the trace file.\n");
    fprintf(stderr, "\t-g         Generate summary info for autograder.\n");
    fprintf(stderr, "\t-h         Print this message.\n");
    fprintf(stderr, "\t-l         Run libc malloc as well.\n");
    fprintf(stderr, "\t-t <dir>   Directory to find default traces.\n");
    fprintf(stderr, "\t-v         Print per-trace performance breakdowns.\n");
    fprintf(stderr, "\t-V         Print additional debug info.\n");
    fprintf(stderr, "\t-n         Don't randomize addresses.\n");
    fprintf(stderr, "\t-s         Vary amplitude of each trace.\n");
    fprintf(stderr, "\t-m <t>     Run with multiple threads (mdriver-ts only).\n");
}
