#!/usr/bin/python
#
# Ctrl-Z Test: Start a shell, run a program, 
#              send SIGTSTP, wait for status msg, move in 
#              foreground, then send SIGINT, then exit
#
# Requires use of the following commands:
#
#    ctrl-z control, fg, sleep
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a command
sendline("sleep 60")

# The following call is necessary to ensure that the SIGTSTP
# we are sending below via 'sendcontrol' reaches the 'sleep' child.
wait_for_fg_child()

#checks the number of active child processes
#using a timeout based process count
proc_check.count_children_timeout(console, 1, 1)

#checks the number of active child processes
#at this moment in time
proc_check.count_active_children(console, 1)

# send SIGTSTP to 'sleep'
sendcontrol('z')

# shell should pick up that 'sleep' was stopped and respond with job status
# it should output a line such as [6]+  Stopped                 (sleep 60)
# (note that the provided regexp assumes the job name appears in parentheses,
# adjust your output_spec.py if needed)
(jobid, statusmsg, cmdline) = parse_job_line()
assert statusmsg == 'stopped', "Shell did not report stopped job"

# move job into foreground
run_builtin('fg', jobid)

# when moving a job in the foreground, bash outputs its command line
expect_exact(cmdline, "Shell did not report the job moved into the foreground")

# send SIGINT
sendintr()

#check that the prompt prints
expect_prompt()

#exit
sendline("exit")
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
