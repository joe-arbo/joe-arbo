/*
 * Thread-safety wrapper.
 * To be included in mm.c - you may assume this file exists, or else
 * (better!) implement your own version.
 *
 * Generally, #including .c files is fragile and not good style.
 * This is just a stop-gap solution.
 */
#include <pthread.h>
#ifdef THREAD_SAFE
static pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;

void *_mm_malloc_thread_unsafe(size_t size);
void _mm_free_thread_unsafe(void *bp);
void *_mm_realloc_thread_unsafe(void *ptr, size_t size);

void *mm_malloc(size_t size)
{
    pthread_mutex_lock(&malloc_lock);
    void * p = _mm_malloc_thread_unsafe(size);
    pthread_mutex_unlock(&malloc_lock);
    return p;
}

void mm_free(void *bp)
{
    pthread_mutex_lock(&malloc_lock);
    _mm_free_thread_unsafe(bp);
    pthread_mutex_unlock(&malloc_lock);
}

void *mm_realloc(void *ptr, size_t size)
{
    pthread_mutex_lock(&malloc_lock);
    void * p = _mm_realloc_thread_unsafe(ptr, size);
    pthread_mutex_unlock(&malloc_lock);
    return p;
}

#define mm_malloc _mm_malloc_thread_unsafe
#define mm_free _mm_free_thread_unsafe
#define mm_realloc _mm_realloc_thread_unsafe

#else
/* If THREAD_SAFE is not defined, we leave it as is in order
 * to avoid the locking overhead. */
#endif /* THREAD_SAFE */
