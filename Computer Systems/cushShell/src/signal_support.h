#ifndef __SIGNAL_SUPPORT_H
#define __SIGNAL_SUPPORT_H

#include <signal.h>
#include <stdbool.h>

/* Signal handler prototype */
typedef void (*sa_sigaction_t)(int, siginfo_t *, void *);

/* Return true if this signal is blocked */
bool signal_is_blocked(int sig);

/* Block a signal. Returns true it was blocked before */
bool signal_block(int sig);

/* Unblock a signal. Returns true it was blocked before */
bool signal_unblock(int sig);

/* Install signal handler for signal 'sig' */
void signal_set_handler(int sig, sa_sigaction_t handler);

#endif /* __SIGNAL_SUPPORT_H */
