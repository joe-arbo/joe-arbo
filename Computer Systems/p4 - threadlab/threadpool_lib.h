#include <time.h>
#include <stdio.h>

/**
 * These functions are mostly used by the tests and test driver.
 */
struct timespec timespec_diff(struct timespec start, struct timespec end);
void timespec_print(struct timespec ts, char *buf, size_t buflen);
int count_number_of_threads(void);

struct benchmark_data;
struct benchmark_data * start_benchmark(void);
void stop_benchmark(struct benchmark_data * bdata);
void report_benchmark_results(struct benchmark_data *bdata);
void report_benchmark_results_to_human(FILE *file, struct benchmark_data *bdata);

/* Worker threads can install this handler to guess whether a segmentation
 * fault may be the result of stack overflow. */
void install_stack_overflow_handler(void);
