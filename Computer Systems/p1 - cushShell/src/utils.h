/* Set the 'close-on-exec' flag on fd, return error indicator */
int utils_set_cloexec(int fd);

/* Print information about the last syscall error */
void utils_error(char *fmt, ...);

/* Print information about the last syscall error and then exit */
void utils_fatal_error(char *fmt, ...);
