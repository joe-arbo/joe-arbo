#!/usr/bin/python
#
# stop_test: tests the stop command
# 
# Test the stop command for stopping a process by its pid.
# Requires the following commands to be implemented
# or otherwise usable:
#
#	stop, sleep
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a command
sendline("sleep 30 &")

# pull the jobid and pid from the background process printout
(jobid, pid) = parse_bg_status()

# ensure that the shell prints the expected prompt
expect_prompt("Shell did not print expected prompt (2)")

#The job needs to be running when we call stop
proc_check.count_children_timeout(console, 1, 1)

# send the stop command to the process
run_builtin('stop', jobid)

#Ensure that sleep has some time to stop before we read its
#/proc/ pid /stat file.
time.sleep(.5)

#Ensure that sleep is now stopped in the background, and is not
#the foreground process.
assert not proc_check.check_pid_fgpgrp(pid), \
                            'Error: process is in the foreground'
assert proc_check.check_pid_status(pid, 'T'), 'Error: process not stopped'

#check the prompt prints
expect_prompt("Shell did not print expected prompt (3)")


sendline("exit");
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
