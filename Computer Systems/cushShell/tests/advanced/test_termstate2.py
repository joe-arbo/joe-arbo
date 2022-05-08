#
# Check that terminal state changes made by 
# a program that does not exit properly don't stick
#
# Note that since cush uses readline (which changes
# the terminal state while running) we must sample the state
# when the shell is running a fg job.
#
from testutils import *
import os, termios, sys, time

c = setup_tests()
expect_prompt()

sendline('sleep 1')
wait_for_fg_child()

time.sleep(.5)
attr_while_running_sleep = termios.tcgetattr(c.child_fd)

expect_prompt()

sendline('vim')
wait_for_fg_child()

time.sleep(.5)
attr_while_running_vim = termios.tcgetattr(c.child_fd)
assert attr_while_running_sleep != attr_while_running_vim

# SIGKILL vim
vim_pid = os.tcgetpgrp(c.child_fd)
os.kill(vim_pid, 9)

# the shell must not sample the state now.  When the next cmd
# is run, the original state from earlier should be used
expect_prompt()

sendline('sleep 1')
wait_for_fg_child()

time.sleep(.5)
attr_while_running_sleep_again = termios.tcgetattr(c.child_fd)
assert attr_while_running_sleep_again == attr_while_running_sleep

expect_prompt()

test_success()
