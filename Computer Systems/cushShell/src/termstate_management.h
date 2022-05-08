#ifndef __TERMSTATE_MANAGEMENT_H
#define __TERMSTATE_MANAGEMENT_H

#include <sys/types.h>

/* Initialize tty support. */
void termstate_init(void);

/* Save current terminal settings.
 * This function should be called when a job is suspended and the
 * state should be saved for this job so it can be restored with
 * termstate_give_terminal_to when that job is made the foreground
 * job again.
 */
void termstate_save(struct termios *saved_tty_state);

/**
 * Assign ownership of the terminal to process group
 * pgrp, restoring its terminal state if provided.
 */
void termstate_give_terminal_to(struct termios *pg_tty_state, pid_t pgrp);

/**
 * Sample the current terminal state as the "last known good
 * state" (that is, the state that is restored when
 * termstate_give_terminal_back_to_shell is called).
 * This should be called when a foreground process exits,
 * but not when a foreground process terminates b/c of a signal.
 */
void termstate_sample(void);

/*
 * Restore the shell's terminal state and assign ownership
 * of the terminal to the shell.  The shell should call this
 * before printing a new prompt.
 */
void termstate_give_terminal_back_to_shell(void);

/* Get a file descriptor that refers to controlling terminal */
int termstate_get_tty_fd(void);

#endif /* __TERMSTATE_MANAGEMENT_H */
