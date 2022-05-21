#ifndef __BUFFER_H_
#define __BUFFER_H_
/*
 * A realloc-based dynamic buffer implementation for C.
 *
 * Written by Scott Pruett.
 *
 * Note: the buffer implementation is similar to Java's StringBuffer
 * in that it supports a dynamically growing array of bytes.
 * It supports append* operations that copy data to the buffer,
 * extending it as necessary.
 *
 * Aiming at efficiency, this buffer is not written as a fully encapsulated
 * data type. Rather, some implementation details are exposed.
 *
 * Because the buffer uses realloc, you cannot keep pointers into the
 * buffer around across calls that may reallocate the buffer.
 *
 * The buffer is not thread-safe.
 * This buffer handles out-of-memory situations by exiting the process.
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char* buf;  // underlying storage
    int len;    // current end; last valid byte is buf[len-1]
    int cap;    // allocated amount of storage
} buffer_t;

/*
 * Initialize a buffer and allocate initialsize storage.
 */
static inline void buffer_init(buffer_t *buf, int initialsize)
{
    buf->len = 0;
    buf->cap = initialsize;
    buf->buf = malloc(buf->cap);
    if (buf->buf == NULL) {
        perror("can't alloc memory: ");
        exit(EXIT_FAILURE);
    }
} 

/* Reset this buffer, truncating storage to size. */
static inline void buffer_reset(buffer_t *buf, int size)
{
    buf->len = 0;
    if (buf->cap > size) {
        buf->buf = realloc(buf->buf, size);
        buf->cap = size;
    }
}

/* Delete this buffer, freeing any storage. */
static inline void buffer_delete(buffer_t *buf)
{
    free(buf->buf);
}

/*
 * To minimize copying, the buffer also has the ability to reserve
 * space for I/O operations, see buffer_ensure_capacity below.
 * In this case, the caller must adjust the buffer's len following
 * the operation.
 *
 * Returns a pointer p to the first usable byte; it is guaranteed that
 * bytes buf[p:p+len] point to usable memory.  Once written, the
 * caller should increment buf.len by the actual number of bytes
 * written into the buffer.
 */
static inline char *buffer_ensure_capacity(buffer_t *buf, int len) {
    if (buf->len + len >= buf->cap) {
        int cap = buf->cap * 2 + len;
        buf->buf = realloc(buf->buf, cap);
        if (buf->buf == NULL) {
            perror("can't alloc memory: ");
            exit(1);
        }
        buf->cap = cap;
    }
    return &buf->buf[buf->len];
}

/* Append mem[0:len] to the buffer, expanding it as necessary. */
static inline void buffer_append(buffer_t *buf, void *mem, int len) {
    buffer_ensure_capacity(buf, len);
    memcpy(&buf->buf[buf->len], mem, len);
    buf->len += len;
}

/* Append c to the buffer, expanding it as necessary. */
static inline void buffer_appendc(buffer_t *buf, char c) {
    buffer_ensure_capacity(buf, 1);
    buf->buf[buf->len] = c;
    buf->len++;
}

/* Append zero-terminated string str to the buffer, expanding it as necessary. */
static inline void buffer_appends(buffer_t *buf, char *str) {
    int len = strlen(str);
    buffer_append(buf, str, len);
}

#endif
