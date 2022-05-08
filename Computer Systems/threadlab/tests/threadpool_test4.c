/*
 * Fork/Join Framework 
 *
 * Test that tasks are actually processed concurrently when more than 2 worker threads
 * are forked.  This test would fail if the framework inadvertently held a global
 * lock during task processing.
 *
 * This is part of the minimum requirements because such a flaw would be considered 
 * disastrous, completely negating the point of a thread pool execution framework.
 *
 * Written by G. Back for CS3214 Spring 2016.
 */
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>

#include "threadpool.h"
#include "threadpool_lib.h"
#define DEFAULT_THREADS 2
#define DEFAULT_TASKS 40

struct time_range {
    uintptr_t taskno;
    struct timeval start;
    struct timeval end;
};

static void *
test_task(struct thread_pool *pool, struct time_range * data)
{
    struct timespec sleep_time = {
        .tv_sec = 0,
        .tv_nsec = 50*1000*1000 // 50 ms should be plenty to allow for overlap
    };
    gettimeofday(&data->start, NULL);
    nanosleep(&sleep_time, NULL);
    gettimeofday(&data->end, NULL);
    return (void *)data->taskno;
}

static int
run_test(int nthreads, int ntasks)
{
    struct benchmark_data * bdata = start_benchmark();
    struct thread_pool * threadpool = thread_pool_new(nthreads);
   
    struct time_range start_end[ntasks];
    struct future * futures[ntasks];

    printf("starting %d tasks...\n", ntasks);
    for (int i = 0; i < ntasks; i++) {
        start_end[i].taskno = i;
        futures[i] = thread_pool_submit(threadpool, (fork_join_task_t) test_task, start_end + i);
    }

    for (int i = 0; i < ntasks; i++) {
        uintptr_t r = (uintptr_t) future_get(futures[i]);
        // consistency check
        if (r != i) {
            fprintf(stderr, "Wrong result, expected %d, got %lu\n", i, r);
            abort();
        }
        future_free(futures[i]);
    }

    thread_pool_shutdown_and_destroy(threadpool);
    stop_benchmark(bdata);

    // now analyze the data. We expect that at least nthreads different
    // tasks were running concurrently.  In other words, there exists a
    // task t that was started while nthreads-1 other tasks had been started,
    // but before each of these tasks finished.
    // Try to find it.

    int best = 0;
    for (int i = 0; i < ntasks; i++) {
        int conc_with_i = 0;
        for (int j = 0; j < ntasks; j++) if (i != j) {
            if (timercmp(&start_end[j].start, &start_end[i].start, <) &&
                timercmp(&start_end[i].start, &start_end[j].end, <)) {
                // j was running while when i was started.
                conc_with_i++;
            }
        }
        if (conc_with_i > best)
            best = conc_with_i;
    }

    if (best < nthreads - 1) {
        fprintf(stderr, "Wrong result, expected that %d tasks execute concurrently when "
            "forked from the main thread, but I could only find a parallelism level of %d\n", 
            nthreads - 1, best);
        abort();
    }

    report_benchmark_results(bdata);
    printf("Test successful.\n");
    free(bdata);
    return 0;
}

/**********************************************************************************/

static void
usage(char *av0, int exvalue)
{
    fprintf(stderr, "Usage: %s [-n <n>] [-t <t>]\n"
                    " -n number of threads in pool, default %d\n"
                    " -t number of tasks, default %d\n"
                    , av0, DEFAULT_THREADS, DEFAULT_TASKS);
    exit(exvalue);
}

int 
main(int ac, char *av[]) 
{
    int c, nthreads = DEFAULT_THREADS, ntasks = DEFAULT_TASKS;
    while ((c = getopt(ac, av, "hn:t:")) != EOF) {
        switch (c) {
        case 'n':
            nthreads = atoi(optarg);
            if (nthreads == 1) {
                fprintf(stderr, "Cannot test for concurrency with only 1 thread\n");
                abort();
            }
            break;
        case 't':
            ntasks = atoi(optarg);
            break;
        case 'h':
            usage(av[0], EXIT_SUCCESS);
        }
    }

    return run_test(nthreads, ntasks);
}
