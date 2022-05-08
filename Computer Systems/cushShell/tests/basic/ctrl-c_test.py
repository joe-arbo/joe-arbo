#!/usr/bin/python
#
# Ctrl-C Test: Start a shell, send SIGINT, run a program, 
#              send SIGINT, then exit
#
# Requires the following commands to be implemented
# or otherwise usable:
#
#	sleep, ctrl-c control
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a command
sendline("sleep 60")

# The following call is necessary to ensure that the SIGINT
# we are sending below reaches the 'sleep' child.
proc_check.wait_until_child_is_in_foreground(console)

#checks that our process is running
proc_check.count_active_children(console, 1)

# send SIGINT
sendintr()

#prompt check
expect_prompt()

#checks that the process was ended
proc_check.count_active_children(console, 0)

sendline("exit")

expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
