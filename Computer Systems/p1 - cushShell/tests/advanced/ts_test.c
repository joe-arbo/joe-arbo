/* 
 * Test that terminal state is properly restored when a process is stopped and restored
 * by the shell, the way ksh does it and POSIX recommends (though does not require). 
 *
 * bash and zsh fail this test. 
 *
 * We change one attribute here: the key for EOF from Ctrl-D to Ctrl-E.
 */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>

#define CTRL_D  4
#define CTRL_E  5

int
main()
{
    int terminal_fd = open(ctermid(NULL), O_RDWR);
    assert (terminal_fd != -1);

    // Step 1. make a change to the terminal state:
    // change the VEOF key from Ctrl-D to Ctrl-E
    struct termios saved_tty_state;
    int rc = tcgetattr(terminal_fd, &saved_tty_state);
    assert (rc == 0);

    assert (saved_tty_state.c_cc[VEOF] == CTRL_D);       // ^D
    saved_tty_state.c_cc[VEOF] = CTRL_E;                 // ^E
    rc = tcsetattr(terminal_fd, TCSANOW, &saved_tty_state);
    assert (rc == 0);

    // Step 2.  Suspend and let user resume
    printf("This job should now stop, please run 'fg' to continue it\n");
    raise(SIGTSTP);
    printf("Job now continuing...\n");

    // Step 3.
    // Expect that job control shell saved the terminal state
    rc = tcgetattr(terminal_fd, &saved_tty_state);
    assert (rc == 0);
    if (saved_tty_state.c_cc[VEOF] != CTRL_E) {
        printf("I expected a POSIX job control shell to preserve my terminal settings\n");
        printf("VEOF was not saved, it is now %d...\n", saved_tty_state.c_cc[VEOF]);
    }

    assert (saved_tty_state.c_cc[VEOF] == CTRL_E);       // ^E

    // now restore it the way it was
    saved_tty_state.c_cc[VEOF] = CTRL_D;                 // ^D
    rc = tcsetattr(terminal_fd, TCSANOW, &saved_tty_state);
    assert (rc == 0);
}
