/*
 * Support for buffered I/O
 *
 * The 'bufio' struct implements buffered I/O for a socket, similar
 * to Java's BufferedReader.
 * 
 * Since it encapsulates a connection's socket, it also provides 
 * methods for sending data.
 *
 * Written by G. Back for CS 3214 Spring 2018
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bufio.h"

/*****************************************************************/
struct bufio {
    int socket;         // underlying socket file descriptor
    size_t bufpos;      // offset of next byte to be read
    buffer_t buf;       // holds data that was received
};

static const int BUFSIZE = 8192;
static const int READSIZE = 2048;
static int min(int a, int b) { return a < b ? a : b; }

/* Create a new bufio object from a socket. */
struct bufio *
bufio_create(int socket)
{
    struct bufio * rc = malloc(sizeof(*rc));
    if (rc == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    rc->bufpos = 0;
    rc->socket = socket;
    buffer_init(&rc->buf, BUFSIZE);
    return rc;
}

/* Close a bufio object, freeing its storage and closing its socket. */
void
bufio_close(struct bufio * self)
{
    if (close(self->socket))
        perror("close");

    buffer_delete(&self->buf);
    free(self);
}

static ssize_t
bytes_buffered(struct bufio *self)
{
    return self->buf.len - self->bufpos;
}

const int TRUNCATE_THRESHOLD = 10000;       // tune me

/* 
 * Discard already read data, shifting any buffered data into a new buffer.
 * This will invalidate all offsets.
 *
 * This method is provided to avoid accumulating all data received
 * on a long-running HTTP/1.1 connection into a single buffer.
 */
void bufio_truncate(struct bufio * self)
{
    if (self->buf.len > TRUNCATE_THRESHOLD) {
        int unread = bytes_buffered(self);
        assert(unread >= 0);
        if (unread == 0) {
            buffer_reset(&self->buf, BUFSIZE);
        } else {
            buffer_t oldbuf = self->buf; 
            buffer_init(&self->buf, BUFSIZE);
            buffer_append(&self->buf, oldbuf.buf + self->bufpos, unread);
            buffer_delete(&oldbuf);
        }
        self->bufpos = 0;
    }
}

static ssize_t
read_more(struct bufio *self)
{
    char * buf = buffer_ensure_capacity(&self->buf, READSIZE);
    int bread = recv(self->socket, buf, READSIZE, MSG_NOSIGNAL);
    if (bread < 1)
        return bread;

    self->buf.len += bread;
    return bread;
}

/* Given an offset into the buffer, return a char *.
 * This pointer will be valid only until the next call 
 * to any of the bufio_read* function.
 *
 * The returned pointer is not guaranteed to point to a
 * zero-terminated string.
 */
char * 
bufio_offset2ptr(struct bufio *self, size_t offset)
{
    assert (offset < self->buf.len);
    return self->buf.buf + offset;
}

/* Given an pointer into the buffer, return an offset
 * that can be used to mark the position of something
 * in the buffer (e.g., a header)
 */
size_t 
bufio_ptr2offset(struct bufio *self, char *ptr)
{
    size_t offset = ptr - self->buf.buf;
    assert (0 <= offset && offset < self->buf.len);
    return offset;
}

/* Read one byte from the socket into the buffer.
 * Returns 0 on EOF, -1 on error, else returns 1.
 */
ssize_t
bufio_readbyte(struct bufio *self, char *out)
{
    if (bytes_buffered(self) == 0) {
        int rc = read_more(self);
        if (rc <= 0)
            return rc;
    }

    *out = self->buf.buf[self->bufpos++];
    return 1;
}

/* Read until newline (\n) is encountered.
 * Sets *line_offset to the buffer offset where the next line
 * starts.
 *
 * Returns number of bytes read, and -1 on error.
 *
 * If EOF is encountered, the line may not be terminated
 * with a \n.
 */
ssize_t 
bufio_readline(struct bufio *self, size_t *line_offset)
{
    *line_offset = self->bufpos;
    char p = 0;
    while (p != '\n') {
        int rc = bufio_readbyte(self, &p);
        if (rc < 0)
            return rc;
        if (rc == 0)
            break;
    }
    return self->bufpos - *line_offset;
}

/* Attempt to read a fixed number of bytes into the buffer.
 * Sets *buf_offset to the offset in the buffer pointing to
 * the first byte read.
 *
 * Returns the actual number of bytes read, or -1 on error.
 */
ssize_t 
bufio_read(struct bufio *self, size_t count, size_t *buf_offset)
{
    *buf_offset = self->bufpos;
    while (bytes_buffered(self) < count) {
        int rc = read_more(self);
        if (rc < 0)
            return rc;
        if (rc == 0)
            break;
    }
    ssize_t bytes_read = min(bytes_buffered(self), count);
    self->bufpos += bytes_read;
    return bytes_read;
}

/* Send a file out to the socket.
 * See sendfile(2) for return value.
 */
ssize_t
bufio_sendfile(struct bufio *self, int fd, off_t *off, size_t filesize)
{
    return sendfile(self->socket, fd, off, filesize);
}

/*
 * Send data contained in 'resp' to the socket.
 * See send(2) for return value.
 */
ssize_t 
bufio_sendbuffer(struct bufio *self, buffer_t * resp)
{
    return send(self->socket, resp->buf, resp->len, MSG_NOSIGNAL);
}
