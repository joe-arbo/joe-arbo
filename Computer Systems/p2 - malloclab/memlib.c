/*
 * memlib.c - a module that simulates the memory system.  Needed because it 
 *            allows us to interleave calls from the student's malloc package 
 *            with the system's malloc package in libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
#include "config.h"

/* private variables */
static char *mem_start_brk;  /* points to first byte of heap */
static char *mem_brk;        /* points to last byte of heap */
static char *mem_max_addr;   /* largest legal heap address */ 
static int use_mmap;         /* Use mmap instead of malloc */
static void * mmap_addr = (void *)0x58000000;

/* 
 * mem_init - initialize the memory system model
 */
void mem_init(int _use_mmap)
{
    use_mmap = _use_mmap;

    /* allocate the storage we will use to model the available VM */
    if (use_mmap) {
        mem_start_brk = (char *)mmap(mmap_addr, MAX_HEAP, PROT_READ|PROT_WRITE, 
                                 MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
        if (mem_start_brk == MAP_FAILED) {
            perror("mem_init_vm: mmap error:");
            exit(1);
        }
        if (mem_start_brk != mmap_addr) {
            perror("mmap");
            fprintf(stderr, 
                "mem_init_vm: could not obtain memory at address %p\n", 
                mmap_addr);
            exit(1);
        }
    } else {
        if ((mem_start_brk = (char *)malloc(MAX_HEAP)) == NULL) {
            fprintf(stderr, "mem_init_vm: malloc error\n");
            exit(1);
        }
    }

    mem_max_addr = mem_start_brk + MAX_HEAP;  /* max legal heap address */
    mem_brk = mem_start_brk;                  /* heap is empty initially */
}

/* 
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void)
{
    if (use_mmap) {
        if (munmap(mem_start_brk, MAX_HEAP))
            perror("munmap");
    } else {
        free(mem_start_brk);
    }
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk()
{
    mem_brk = mem_start_brk;
}

/* 
 * mem_sbrk - simple model of the sbrk function. Extends the heap 
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
void *mem_sbrk(int incr) 
{
    char *old_brk = mem_brk;

    if ( (incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
	errno = ENOMEM;
	fprintf(stderr, "ERROR: mem_sbrk(%d) failed. Ran out of memory...\n", incr);
	return NULL;
    }
    mem_brk += incr;
    return (void *)old_brk;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo()
{
    return (void *)mem_start_brk;
}

/* 
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() 
{
    return (size_t)(mem_brk - mem_start_brk);
}

/*
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}
