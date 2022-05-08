#!/usr/bin/python
#
# bg_test: tests the bg command
# 
# Test the stop command for stopping a process by its pid.
# Requires the following commands to be implemented
# or otherwise usable:
#
#	bg, sleep, stop
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run program sleep
sendline("sleep 30 &")

#Used to get the jobid and pid of the sleep process
(jobid, pid) = parse_bg_status()

# ensure that shell prints expected prompt
expect_prompt("Shell did not print expected prompt (2)")

# send the stop command to the process
run_builtin('stop', jobid)

#Ensure that sleep has enough time to stop before we read its
#/proc/ /stat file.
proc_check.count_children_timeout(console, 1, 1)

#Ensure that sleep is now stopped in the background, and is not
#the foreground process.
assert not proc_check.check_pid_fgpgrp(pid), \
                            'Error: process is in the foreground'
assert proc_check.check_pid_status(pid, 'T'), 'Error: process not stopped'

#check the prompt prints
expect_prompt("Shell did not print expected prompt (3)")

#resume the sleep program
run_builtin('bg', jobid)

#check the prompt prints
expect_prompt("Shell did not print expected prompt (4)")

#Ensure that sleep has enough time to start before we read its
#/proc/ /stat file.
proc_check.count_children_timeout(console, 1, 1)

#Ensure that sleep is running now in the background, and is not
#the foreground process.
assert not proc_check.check_pid_fgpgrp(pid), \
                            'Error: process is in the foreground'
assert proc_check.check_pid_status(pid, 'S'), 'Error: process not running'


#exit
sendline("exit");
expect("exit\r\n", "Shell output extraneous characters")

test_success()
