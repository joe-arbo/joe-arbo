#include <stdio.h>
#include <stdbool.h>

/*  This file aids in the dynamic instrumentation of the heap visualizer.
 *  Do not make changes to this file unless you know what you're doing.
 */

/* Create typedef for our callback function */
typedef void (*memory_analyzer_callback_t)(void *, void *, long long, int, char *, char *);


/* Create typedef for the list callback */
typedef void (*memory_analyzer_list_init_callback_t)(void *);

/* Create typedef for the list callback */
typedef void (*memory_analyzer_list_integrity_callback_t)(void);

/* Create typedef for memory load detected */
typedef void (*memory_load_detected_callback_t)(void *);

/* Function signatures */
void __memory_write_detected(void *, void *, long long, int, char *, char *);
void __list_integrity_check(void);
void __list_init_detected(void *);
void __memory_load_detected(void *);

/* Create file-scope variables for our analyzer */
static bool __memory_analyzer_initialized = false;
static memory_analyzer_callback_t __memory_analyzer_function_callback = NULL;
static memory_analyzer_list_init_callback_t __memory_analyzer_list_callback = NULL;
static memory_analyzer_list_integrity_callback_t __memory_analyzer_list_integrity_callback = NULL;
static memory_load_detected_callback_t __memory_load_detected_callback = NULL;

void initialize_memory_analyzer(memory_analyzer_callback_t callback, memory_analyzer_list_init_callback_t list_callback, memory_analyzer_list_integrity_callback_t list_integrity_callback, memory_load_detected_callback_t memory_load_callback) {
    __memory_analyzer_function_callback = callback;
    __memory_analyzer_list_callback = list_callback;
    __memory_analyzer_list_integrity_callback = list_integrity_callback;
    __memory_load_detected_callback = memory_load_callback;
    __memory_analyzer_initialized = true;
}

void __memory_load_detected(void * pointer) {
    if (__memory_analyzer_initialized) {
        __memory_load_detected_callback(pointer);
    }
}

void __memory_write_detected(void * pointer, void * value, long long size, int line_number, char * filename, char * structname) {
    /* If the memory analyzer is initialized, create a function call to our callback */
    if (__memory_analyzer_initialized) {
        __memory_analyzer_function_callback(pointer, value, size, line_number, filename, structname);
    }
}

void __list_init_detected(void * list) {
    if (__memory_analyzer_initialized) {
        __memory_analyzer_list_callback(list);
    }
}

void __list_integrity_check(void) {
    if (__memory_analyzer_initialized) {
        __memory_analyzer_list_integrity_callback();
    }
}
