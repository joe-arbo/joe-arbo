#
# fg_test: tests the fg command
# 
# Test the fg command for bringing a command back to the foreground.
# Requires the following commands to be implemented
# or otherwise usable:
#
#	fg, sleep, ctrl-c control, ctrl-z control
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
proc_check.wait_until_child_is_in_foreground(console)

# send SIGTSTP to 'sleep'
sendcontrol('z')

# shell should pick up that 'sleep' was stopped and respond with job status
# it should output a line such [6]+  Stopped                 sleep 60
(jobid, status, cmdline) = parse_job_line()
assert status == 'stopped', "Shell did not report stopped job"

# move job into foreground
run_builtin('fg', jobid)

# when moving a job in the foreground, bash outputs its command line
expect_exact(cmdline, "Shell did not report the job moved into the foreground")

# end the program
sendintr()

# run a command
sendline("sleep 30 &")

#snag the jobid and pid of the sleep command
(jobid, pid) = parse_bg_status()

#check the prompt prints
expect_prompt("Shell did not print expected prompt (2)")

#resume the sleep command
run_builtin('fg', jobid)

#wait until it takes over the foreground process group
proc_check.wait_until_child_is_in_foreground(console)

#send the command back to the background
sendcontrol('z')

#check the prompt prints
expect_prompt("Shell did not print expected prompt (3)")

#run a command to the background
sendline("sleep 300 &")

#snag the jobid and pid of the second sleep command
(jobid2, pid2) = parse_bg_status()

#check the prompt prints
expect_prompt("Shell did not print expected prompt (4)")

#resume the command by its jobid
run_builtin('fg', jobid)

#wait until it takes over the foreground process group
proc_check.wait_until_child_is_in_foreground(console)

#Ensure that the sleep is in the foreground process group via /proc/
assert proc_check.check_pid_fgpgrp(pid),  "Error, the pid's process group is \
                                           not the foreground process group"

#send the command back to the background
sendcontrol('z')

#check the prompt prints
expect_prompt("Shell did not print expected prompt (5)")

#resume the command by its jobid
run_builtin('fg', jobid2)

#wait until it takes over the foreground process group
proc_check.wait_until_child_is_in_foreground(console)

#Ensure that the sleep is in the foreground process group via /proc/
assert proc_check.check_pid_fgpgrp(pid2),  "Error, the pid's process group is \
                                           not the foreground process group"

#end the process
sendintr()

#check that the prompt prints
expect_prompt("Shell did not print expected prompt (6)")

#resume the first sleep command
run_builtin('fg', jobid)

#wait until the process takes over the foreground process group
proc_check.wait_until_child_is_in_foreground(console)

#Ensure that the sleep is in the foreground process group via /proc/
assert proc_check.check_pid_fgpgrp(pid),  "Error, the pid's process group is \
                                           not the foreground process group"

#exit
sendline("exit");
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
