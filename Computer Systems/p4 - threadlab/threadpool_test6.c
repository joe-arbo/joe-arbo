/*
 * Fork/Join Framework 
 *
 * Test that future_free() is implemented and working.
 * For this test, we expect that any memory allocated by your
 * threadpool implementation is freed when the program exits.
 *
 * While it is normally ok to not free memory before exiting,
 * libraries that are intended for long-running programs, such
 * as the threadpool library in this project, cannot leak
 * memory during normal operation.
 *
 * Thus, this test will succeed only if valgrind reports this:
 *
 * ==2336955== LEAK SUMMARY:
 * ==2336955==    definitely lost: 0 bytes in 0 blocks
 * ==2336955==    indirectly lost: 0 bytes in 0 blocks
 * ==2336955==      possibly lost: 0 bytes in 0 blocks
 * ==2336955==    still reachable: 0 bytes in 0 blocks
 *
 * Written by G. Back for CS3214 Summer 2020.
 * Updated Fall 2020.
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
#define DEFAULT_TASKS 50

struct taskno_wrapper {
    uintptr_t taskno;
};

static void *
inner_task(struct thread_pool *pool, struct taskno_wrapper * data)
{
    return (void *)data->taskno;
}

static void *
test_task(struct thread_pool *pool, struct taskno_wrapper * data)
{
    struct taskno_wrapper childdata = { .taskno = 2 * data->taskno };
    struct future * child = thread_pool_submit(pool, (fork_join_task_t) inner_task, &childdata);
    void * result = future_get(child);
    if ((uintptr_t) result != 2 * data->taskno)
        abort();
    future_free(child);
    return (void *)data->taskno;
}

static int
run_test(int nthreads, int ntasks)
{
    struct benchmark_data * bdata = start_benchmark();
    struct thread_pool * threadpool = thread_pool_new(nthreads);
   
    struct taskno_wrapper  * task_data = malloc(sizeof(*task_data) * ntasks);
    struct future ** futures = malloc(sizeof(*futures) * ntasks);

    printf("starting %d tasks...\n", ntasks);
    for (int i = 0; i < ntasks; i++) {
        task_data[i].taskno = i;
        futures[i] = thread_pool_submit(threadpool, (fork_join_task_t) test_task, task_data + i);
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
    free(task_data);
    free(futures);

    thread_pool_shutdown_and_destroy(threadpool);
    stop_benchmark(bdata);

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
