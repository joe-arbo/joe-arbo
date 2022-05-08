/* 
 * Simple, 64-bit allocator based on implicit free lists, 
 * first fit placement, and boundary tag coalescing, as described
 * in the CS:APP2e text. Blocks must be aligned to 16 byte
 * boundaries. Minimum block size is 16 bytes. 
 *
 * This version is loosely based on 
 * http://csapp.cs.cmu.edu/3e/ics3/code/vm/malloc/mm.c
 * but unlike the book's version, it does not use C preprocessor 
 * macros or explicit bit operations.
 *
 * It follows the book in counting in units of 4-byte words,
 * but note that this is a choice (my actual solution chooses
 * to count everything in bytes instead.)
 *
 * You may use this code as a starting point for your implementation
 * if you want.
 *
 * Adapted for CS3214 Summer 2020 by gback
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "mm.h"
#include "memlib.h"
#include "config.h"
#include "list.h" //for explicit list

struct boundary_tag {
    int inuse:1;        // inuse bit
    int size:31;        // size of block, in words
                        // block size
};

/* FENCE is used for heap prologue/epilogue. */
const struct boundary_tag FENCE = {
    .inuse = 1,
    .size = 0
};

/* A C struct describing the beginning of each block. 
 * For implicit lists, used and free blocks have the same 
 * structure, so one struct will suffice for this example.
 *
 * If each block is aligned at 12 mod 16, each payload will
 * be aligned at 0 mod 16.
 */
struct block {
    struct boundary_tag header; /* offset 0, at address 12 mod 16 */
    char payload[0];            /* offset 4, at address 0 mod 16 */
    struct list_elem elem;
};

/* Basic constants and macros */
#define WSIZE       sizeof(struct boundary_tag)  /* Word and header/footer size (bytes) */
#define MIN_BLOCK_SIZE_WORDS 12  /* Minimum block size in words */ /*changed from 4 to 12, maintains alignment*/
#define CHUNKSIZE  (1<<5)  /* Extend heap by this amount (words) */
#define NUM_LISTS 7 /* Number of segregated lists */

static inline size_t max(size_t x, size_t y) {
    return x > y ? x : y;
}

static size_t align(size_t size) {
  return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

static bool is_aligned(size_t size) __attribute__((__unused__));
static bool is_aligned(size_t size) {
  return size % ALIGNMENT == 0;
}

/* Global variables */
static struct block *heap_listp = 0;  /* Pointer to first block */  
static struct list seg_list[NUM_LISTS]; /* 32, 64, 128, 256, 512, 1024, max 
                                 indexes:   0   1   2    3    4     5    6 */

/* Function prototypes for internal helper routines */
static struct block *extend_heap(size_t words);
static void place(struct block *bp, size_t asize);
static struct block *find_fit(size_t asize);
static struct block *coalesce(struct block *bp);
static void seg_push(struct block *bp);

/* Given a block, obtain previous's block footer.
   Works for left-most block also. */
static struct boundary_tag * prev_blk_footer(struct block *blk) {
    return &blk->header - 1;
}

/* Return if block is free */
static bool blk_free(struct block *blk) { 
    return !blk->header.inuse; 
}

/* Return size of block is free */
static size_t blk_size(struct block *blk) { 
    return blk->header.size; 
}

/* Given a block, obtain pointer to previous block.
   Not meaningful for left-most block. */
static struct block *prev_blk(struct block *blk) {
    struct boundary_tag *prevfooter = prev_blk_footer(blk);
    assert(prevfooter->size != 0);
    return (struct block *)((void *)blk - WSIZE * prevfooter->size);
}

/* Given a block, obtain pointer to next block.
   Not meaningful for right-most block. */
static struct block *next_blk(struct block *blk) {
    assert(blk_size(blk) != 0);
    return (struct block *)((void *)blk + WSIZE * blk->header.size);
}

/* Given a block, obtain its footer boundary tag */
static struct boundary_tag * get_footer(struct block *blk) {
    return ((void *)blk + WSIZE * blk->header.size)
                   - sizeof(struct boundary_tag);
}

/* Set a block's size and inuse bit in header and footer */
static void set_header_and_footer(struct block *blk, int size, int inuse) {
    blk->header.inuse = inuse;
    blk->header.size = size;
    * get_footer(blk) = blk->header;    /* Copy header to footer */
}

/* Mark a block as used and set its size. */
static void mark_block_used(struct block *blk, int size) {
    set_header_and_footer(blk, size, 1);
}

/* Mark a block as free and set its size. */
static void mark_block_free(struct block *blk, int size) {
    set_header_and_footer(blk, size, 0);
}

/* 
 * mm_init - Initialize the memory manager 
 */
int mm_init(void) 
{
    /* Initialize segregated lists */
    for (int i = 0; i < NUM_LISTS; ++i){
        list_init(&seg_list[i]);
    }
    assert (offsetof(struct block, payload) == 4);
    assert (sizeof(struct boundary_tag) == 4);
    /* Create the initial empty heap */
    struct boundary_tag * initial = mem_sbrk(4 * sizeof(struct boundary_tag));
    if (initial == (void *)-1)
        return -1;

    /* We use a slightly different strategy than suggested in the book.
     * Rather than placing a min-sized prologue block at the beginning
     * of the heap, we simply place two fences.
     * The consequence is that coalesce() must call prev_blk_footer()
     * and not prev_blk() because prev_blk() cannot be called on the
     * left-most block.
     */
    initial[2] = FENCE;                     /* Prologue footer */
    heap_listp = (struct block *)&initial[3];
    initial[3] = FENCE;                     /* Epilogue header */

    struct block * blk = NULL;

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if ((blk = extend_heap(CHUNKSIZE)) == NULL) 
        return -1;
    //list_push_back(&free_list, &blk->elem); /*put the initial chunk in the list*/
    seg_push(blk);
    return 0;
}

/* 
 * mm_malloc - Allocate a block with at least size bytes of payload 
 */
void *mm_malloc(size_t size)
{
    struct block *bp;      

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    size += 2 * sizeof(struct boundary_tag);    /* account for tags */
    /* Adjusted block size in words */
    size_t awords = max(MIN_BLOCK_SIZE_WORDS, align(size)/WSIZE); /* respect minimum size */

    /* Search the free list for a fit */
    if ((bp = find_fit(awords)) != NULL) {
        place(bp, awords);
        return bp->payload;
    }

    /* No fit found. Get more memory and place the block */
    size_t extendwords = max(awords,CHUNKSIZE); /* Amount to extend heap if no fit */
    if ((bp = extend_heap(extendwords)) == NULL)  
        return NULL;

    place(bp, awords);
    return bp->payload;
} 

/* 
 * mm_free - Free a block 
 */
void mm_free(void *bp)
{
    assert (heap_listp != 0);       // assert that mm_init was called
    if (bp == 0) 
        return;

    /* Find block from user pointer */
    struct block *blk = bp - offsetof(struct block, payload);
    mark_block_free(blk, blk_size(blk));
    blk = coalesce(blk);
    seg_push(blk);
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static struct block *coalesce(struct block *bp) 
{
    bool prev_alloc = prev_blk_footer(bp)->inuse;   /* is previous block allocated? */
    bool next_alloc = ! blk_free(next_blk(bp));     /* is next block allocated? */
    size_t size = blk_size(bp);

    if (prev_alloc && next_alloc) {            /* Case 1 */
        // both are allocated, nothing to coalesce
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        // combine this block and next block by extending it
        list_remove(&next_blk(bp)->elem);
        mark_block_free(bp, size + blk_size(next_blk(bp)));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        // combine previous and this block by extending previous
        list_remove(&prev_blk(bp)->elem);
        bp = prev_blk(bp);
        mark_block_free(bp, size + blk_size(bp));
    }

    else {                                     /* Case 4 */
        // combine all previous, this, and next block into one
        list_remove(&prev_blk(bp)->elem);
        list_remove(&next_blk(bp)->elem);
        mark_block_free(prev_blk(bp), 
                        size + blk_size(next_blk(bp)) + blk_size(prev_blk(bp)));
        bp = prev_blk(bp);
    }
    return bp;
}

/*
 * mm_realloc - Naive implementation of realloc
 */
void *mm_realloc(void *ptr, size_t size)
{
    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL) {
        return mm_malloc(size);
    }

    struct block *oldblock = ptr - offsetof(struct block, payload);
    size_t oldsize = blk_size(oldblock) * WSIZE;
    
    size += 2 * sizeof(struct boundary_tag);    /* account for tags */
    size_t udjustedSize = align(size)/WSIZE;

    // CASE 0
    if (oldsize >= size)
    {
        size_t awords = max(MIN_BLOCK_SIZE_WORDS, udjustedSize);
        place(oldblock, awords);
        return oldblock->payload;
    }
    
    // CASE 1
    if (((oldsize + (blk_size(next_blk(oldblock)) * WSIZE)) >= size) && blk_free(next_blk(oldblock)))
    {
        list_remove(&next_blk(oldblock)->elem);
        mark_block_used(oldblock, (oldsize/WSIZE) + blk_size(next_blk(oldblock)));
        size_t awords = max(MIN_BLOCK_SIZE_WORDS, udjustedSize);
        place(oldblock, awords);
        return oldblock->payload;
    }
   
    // CASE 2
     if (((oldsize + (blk_size(next_blk(oldblock)) * WSIZE) + (blk_size(prev_blk(oldblock)) * WSIZE)) >= size) 
        && blk_free(next_blk(oldblock)) && blk_free(prev_blk(oldblock)))
    {
        list_remove(&next_blk(oldblock)->elem);
        list_remove(&prev_blk(oldblock)->elem);
        mark_block_used(prev_blk(oldblock), (oldsize/WSIZE) + blk_size(next_blk(oldblock)) + blk_size(prev_blk(oldblock)));
        oldblock = prev_blk(oldblock);
        memmove(oldblock->payload, ptr, size);
        size_t awords = max(MIN_BLOCK_SIZE_WORDS, udjustedSize);
        place(oldblock, awords);
        return oldblock->payload;
    }

    // CASE 3
    if (blk_size(next_blk(oldblock)) == 0) //maybe add inuse
    {
        size_t extendSize = max(MIN_BLOCK_SIZE_WORDS, align(size - oldsize) / WSIZE);
        extend_heap(extendSize);
        mark_block_used(oldblock, (oldsize/WSIZE) + blk_size(next_blk(oldblock)));
        size_t awords = max(MIN_BLOCK_SIZE_WORDS, udjustedSize);
        place(oldblock, awords);
        return oldblock->payload;
    }

    // CASE 4
    if (((oldsize + (blk_size(prev_blk(oldblock)) * WSIZE)) >= size) && blk_free(prev_blk(oldblock)))
    {
        list_remove(&prev_blk(oldblock)->elem);
        mark_block_used(prev_blk(oldblock), (oldsize/WSIZE) + blk_size(prev_blk(oldblock)));
        oldblock = prev_blk(oldblock);
        memmove(oldblock->payload, ptr, size);
        size_t awords = max(MIN_BLOCK_SIZE_WORDS, udjustedSize);
        place(oldblock, awords);
        return oldblock->payload;
    }
    
    void *newptr = mm_malloc(size);

    /* If realloc() fails the original block is left untouched  */
    if (!newptr) {
        return 0;
    }

    /* Copy the old data. */
    if (size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    /* Free the old block. */
    mm_free(ptr);

    return newptr;
}

/* 
 * checkheap - We don't check anything right now. 
 */
void mm_checkheap(int verbose)
{ 
}

/* 
 * The remaining routines are internal helper routines 
 */

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
static struct block *extend_heap(size_t words) 
{
    void *bp = mem_sbrk(words * WSIZE);

    if ((intptr_t) bp == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header.
     * Note that we overwrite the previous epilogue here. */
    struct block * blk = bp - sizeof(FENCE);
    mark_block_free(blk, words);
    next_blk(blk)->header = FENCE;

    /* Coalesce if the previous block was free */
    return coalesce(blk);
}

/* 
 * place - Place block of asize words at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(struct block *bp, size_t asize)
{
    size_t csize = blk_size(bp);

    if ((csize - asize) >= MIN_BLOCK_SIZE_WORDS) { 
        mark_block_used(bp, asize);
        bp = next_blk(bp);
        mark_block_free(bp, csize-asize);
        seg_push(bp);
    }
    else { 
        mark_block_used(bp, csize);
    }
}

/* 
 * find_fit - Find a fit for a block with asize words 
 */
static struct block *find_fit(size_t asize)
{
    /* First fit search */
    /* Overshoot to improve hit likelihood */
    /* Idea here is that you can "skip" the earlier lists
        by just setting the start index to be higher */
    int i = 0;
    if (asize >= 16){
        i = 1;
    }
    if (asize >= 32){
        i = 2;
    }
    if (asize >= 64){
        i = 3;
    }
    if (asize >= 128){
        i = 4;
    }
    if (asize >= 256){
        i = 5;
    }
    if (asize >= 512){
        i = 6;
    }

    /* Check each valid list */
    for(; i < NUM_LISTS; ++i){
        struct list_elem *e;
        for (e = list_begin (&seg_list[i]); e != list_end (&seg_list[i]);
            e = list_next (e))
        {
            struct block *f = list_entry (e, struct block, elem);
            if (asize <= blk_size(f)) {
                list_remove(&f->elem);
                return f;
            }
        }
    }
    return NULL; /* No fit */
}

/*
* Pushes the provided block to the appropriate
* segregated list, based on its size.
*/
static void seg_push(struct block *bp){
    size_t size = blk_size(bp);
    if (size <= 32){
        list_push_back(&seg_list[0], &bp->elem);
    }
    else if (size <= 64){
        list_push_back(&seg_list[1], &bp->elem);
    }
    else if (size <= 128){
        list_push_back(&seg_list[2], &bp->elem);
    }
    else if (size <= 256){
        list_push_back(&seg_list[3], &bp->elem);
    }
    else if (size <= 512){
        list_push_back(&seg_list[4], &bp->elem);
    }
    else if (size <= 1024){
        list_push_back(&seg_list[5], &bp->elem);
    }
    else{ //catch-all for larger sizes
        list_push_back(&seg_list[6], &bp->elem);
    }
}

team_t team = {
    /* Team name */
    "Segregated list implemenation for malloc",
    /* First member's full name */
    "Rio Cross",
    "riocross@vt.edu",
    /* Second member's full name (leave blank if none) */
    "Joseph Arbolino",
    "joearbo13@vt.edu",
};
