/*
 * Utility functions for printing error messages
 */

#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <assert.h>

#include "utils.h"

/* Utility function for utils_fatal_error and utils_error */
static void 
print_error_helper(char *fmt, va_list ap)
{
    char errmsg[1024];

    strerror_r(errno, errmsg, sizeof errmsg);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "%s\n", errmsg);
}

/* Print information about the last syscall error */
void 
utils_error(char *fmt, ...) 
{
    va_list ap;
    va_start(ap, fmt);
    print_error_helper(fmt, ap);
    va_end(ap);
}

/* Print information about the last syscall error and then exit */
void
utils_fatal_error(char *fmt, ...) 
{
    va_list ap;
    va_start(ap, fmt);
    print_error_helper(fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}


/* Set the 'close-on-exec' flag on fd, return error indicator */
int
utils_set_cloexec(int fd)
{
    int oldflags = fcntl (fd, F_GETFD, 0);
    if (oldflags < 0)
        return oldflags;

    return fcntl(fd, F_SETFD, oldflags | FD_CLOEXEC);
}

