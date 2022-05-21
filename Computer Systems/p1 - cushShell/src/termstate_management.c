/*
 * Utility functions to support managing the terminal 
 * state for a job control shell.
 *
 * Refactored for CS 3214 Summer 2020 Virginia Tech.
 */

#include <termios.h>
#include <errno.h>
#include <stddef.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "termstate_management.h"
#include "utils.h"
#include "signal_support.h"

static int terminal_fd = -1;           /* The controlling terminal */
static struct termios saved_tty_state; /* The state of the terminal when shell
                                           was started. */
static int shell_pgrp;          /* The pgrp of the shell when it started */

/* Initialize tty support. */
void
termstate_init(void)
{
    char *tty;
    assert(terminal_fd == -1 || !!!"termstate_init already called");

    terminal_fd = open(tty = ctermid(NULL), O_RDWR);
    if (terminal_fd == -1)
        utils_fatal_error("opening controlling terminal %s failed: ", tty);

    if (utils_set_cloexec(terminal_fd))
        utils_fatal_error("cannot mark terminal fd FD_CLOEXEC");

    shell_pgrp = getpgrp();
    termstate_sample();
}

/* Save current terminal settings.
 * This function is used when a job is suspended.*/
void 
termstate_save(struct termios *saved_tty_state)
{
    int rc = tcgetattr(terminal_fd, saved_tty_state);
    if (rc == -1)
        utils_fatal_error("tcgetattr failed: ");
}

/* Restore terminal to saved settings.
 * This function is used when resuming a suspended job. */
static void
termstate_restore(struct termios *saved_tty_state)
{
    int rc;

retry:
    rc = tcsetattr(terminal_fd, TCSADRAIN, saved_tty_state);
    if (rc == -1) {
        /* tcsetattr, apparently, does not restart even with SA_RESTART,
         * so repeat call on EINTR. */
        if (errno == EINTR)
            goto retry;

        utils_fatal_error("could not restore tty attributes tcsetattr: ");
    }
}

/* Get a file descriptor that refers to controlling terminal */
int
termstate_get_tty_fd(void)
{
    assert(terminal_fd != -1 || !!!"termstate_init() must be called");
    return terminal_fd;
}

/**
 * Assign ownership of the terminal to process group
 * pgrp, restoring its terminal state if provided.
 *
 * Before printing a new prompt, the shell should
 * invoke this function with its own process group
 * id (obtained on startup via getpgrp()) and a
 * sane terminal state (obtained on startup via
 * termstate_init()).
 */
void
termstate_give_terminal_to(struct termios *pg_tty_state, pid_t pgrp)
{
    signal_block(SIGTTOU);
    int rc = tcsetpgrp(termstate_get_tty_fd(), pgrp);
    if (rc == -1)
        utils_fatal_error("tcsetpgrp: ");

    if (pg_tty_state)
        termstate_restore(pg_tty_state);
    signal_unblock(SIGTTOU);
}

void 
termstate_give_terminal_back_to_shell(void)
{
    assert (shell_pgrp > 0 || !!!"termstate_init was not called");
    termstate_give_terminal_to(&saved_tty_state, shell_pgrp);
}

void
termstate_sample(void)
{
    termstate_save(&saved_tty_state);
}
