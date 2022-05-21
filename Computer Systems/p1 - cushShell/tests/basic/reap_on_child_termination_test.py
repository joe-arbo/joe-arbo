#!/usr/bin/python
#
# reap_on_child_termination_test: 
# Tests that a background process will be reaped when it finishes
# even when the shell is waiting for user input with readline. 
# This requires the SIGCHLD to not be blocked when waiting on user input
# We want to have this behaviour so that the process resources are not tied up unnecessarily.
# 
#
#
# 
# Requires the following commands to be implemented
# or otherwise usable:
#
#	sleep
#
# In addition the shell must be capable of running processes in the background.
# This includes giving the proper output when running a command in the background
# this is of the form:
#
# [jobid] pid
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a sleep command for enough time to allow the prompt to return to the shell
sendline("sleep 2 &")

# parse the jobid and pid output
(jobid, pid) = parse_bg_status()

# ensure that the shell prints the expected prompt within a reasonable time
expect_prompt("Shell did not print expected prompt (2)")

# The job needs to be running when it prints the prompt so that when it dies we can see if it reaps it
# If it takes more than 2 seconds to give the prompt back to the user after typing in sleep 2 & then 
# the shell should be deemed much too slow anyways and will fail the test
# we do not expect 2 seconds to be a time student shells will come close to at all.
proc_check.count_children_timeout(console, 1, 1)

# sleep for long enough to ensure that the sleep program has terminated and should have been reaped
time.sleep(3.5)

# check the proc file that the process has actually been reaped
# the proc file should not exist once it has been reaped
# Note that this is checking for the existence of the /proc file
# outside of the shell that spawned the command. This can be simulated by students
# by opening up another shell and looking at /proc
assert not os.path.exists("/proc/" + pid + "/stat"), 'the process was not \
reaped'

dne = "/askdjfhalsdhjlajksdhflas"
sendline(dne)
console.ignorecase = True
expect(dne)
expect('no such file or directory')

sendline("exit");

# ensure that no extra characters are output after exiting
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
