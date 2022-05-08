#!/usr/bin/python
#
# jobs_test: tests the jobs command
# 
# Test the jobs command for status messages and proper output
# of the command and jobid.  Requires the following commands to be implemented
# or otherwise usable:
#
#	jobs, ctrl-z control, ctrl-c control, sleep, fg, clear
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

#check that the jobs list outputs nothing
run_builtin('jobs')

# ensure that shell prints expected prompt
expect_prompt("Shell did not print expected prompt (2)")

# run a command
sendline("sleep 30")

#Wait until the sleep command takes over the foreground
proc_check.wait_until_child_is_in_foreground(console)

#send the job to the background
sendcontrol('z')

# ensure that the shell prints the expected prompt
expect_prompt("Shell did not print expected prompt (3)")

#Request the jobs list
run_builtin('jobs')

#Check the jobs list
(jobid, status_message, command_line) = parse_job_line()
assert status_message == 'stopped' and \
		'sleep 30' in command_line, "Job status not properly displayed"

# ensure that the shell prints the expected prompt
expect_prompt("Shell did not print expected prompt (4)")

#Add another job
sendline("sleep 300 &")

# pick up the background job output
(jobid, pid) = parse_bg_status()

expect_prompt("Shell did not print expected prompt (5)")

#Both jobs need to be active and running before the jobs command is
#sent.  if this isn't so, the test is failed.
proc_check.count_active_children(console, 2)

#Recheck the jobs list
run_builtin('jobs')

#Check the jobs list
(jobid, status_message, command_line) = parse_job_line()
(jobid2, status_message2, command_line2) = parse_job_line()

# Check that the jobs list contains both jobs in some order

#check the first possible order of job statuses, and then
#the second possible order.
assert  (status_message == 'stopped' and \
		'sleep 30' in command_line and \
		\
		status_message2 == 'running' and \
		'sleep 300' in command_line2) \
		\
		or \
		\
		(status_message2 == 'stopped' and \
		'sleep 30' in command_line2 and \
		\
		status_message == 'running' and \
		'sleep 300' in command_line), "Job status not properly displayed"

# Check that there are no duplicate job id's.
assert jobid != jobid2, "Duplicate job id's."

expect_prompt("Shell did not print expected prompt (6)")

#bring the second sleep command back to foreground 
#so that we can end it with ctrl-c
run_builtin('fg', jobid2)

#Wait until the sleep command takes over the foreground
proc_check.wait_until_child_is_in_foreground(console)

#ctrl-c to close the process
sendintr()

#clear any printout of the old job that was just killed by ctrl-c
run_builtin('jobs')

#check the prompt and move past this text
expect_prompt("Shell did not print expected prompt (7)")

expect_prompt("Shell did not print expected prompt (8)")

#check the jobs list
run_builtin('jobs')

#check that the first job is still on the jobs list
assert (jobid, status_message, command_line) == parse_job_line(),\
    "The original job was not displayed properly after ending a previous job."

# ensure the prompt is printed
expect_prompt("Shell did not print expected prompt (9)")

# exit
sendline("exit");
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
