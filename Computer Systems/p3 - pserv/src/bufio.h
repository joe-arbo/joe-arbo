#ifndef _BUFIO_H
#define _BUFIO_H

#include "buffer.h"

struct bufio;   // opaque type
                // users should interact only via the public functions below
struct bufio * bufio_create(int socket);
void bufio_close(struct bufio * self);
void bufio_truncate(struct bufio * self);
ssize_t bufio_readbyte(struct bufio *self, char *out);
ssize_t bufio_readline(struct bufio *self, size_t *line_offset);
ssize_t bufio_read(struct bufio *self, size_t count, size_t *buf_offset);
char * bufio_offset2ptr(struct bufio *self, size_t offset);
size_t bufio_ptr2offset(struct bufio *self, char *ptr);
ssize_t bufio_sendfile(struct bufio *self, int fd, off_t *off, size_t filesize);
ssize_t bufio_sendbuffer(struct bufio *self, buffer_t *response);

#endif /* _BUFIO_H */
