#
# kill_test: tests the kill command with the default
# semantics of:
#
# kill <jid>
#
# This test may require updating such that we test other signals
# 
# Requires the following commands to be implemented
# or otherwise usable:
#
#	kill, sleep
#

import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a command
sendline("sleep 30 &")

# parse the jobid and pid output
(jobid, pid) = parse_bg_status()

# ensure that the shell prints the expected prompt
expect_prompt("Shell did not print expected prompt (2)")

# The job needs to be running when we call kill
proc_check.count_children_timeout(console, 1, 1)

# Run the kill command and kill the sleep process in the background
run_builtin('kill', jobid)

# ensure that the shell prints the expected prompt
expect_prompt("Shell did not print expected prompt (3)")

# ensure there is enough time for the process to be killed
time.sleep(.5)

# check the proc file that the process has actually been stopped
# the proc file should not exist
#
# please note that the OS will remove this entry only after the process
# has been both killed and reaped.  (If it's just killed but not reaped,
# it's a Zombie but retains its /proc entry.)
# Make sure your shell does not with SIGCHLD blocked at the prompt
# and fails to reap the sleep 30 & background job from above after you
# kill it.
#
assert not os.path.exists("/proc/" + pid + "/stat"), 'the process was not \
killed'

exe = make_test_program(open(os.path.dirname(__file__) + "/blocks_sigint.c").read())

sendline('{0}'.format(exe))
childpid = proc_check.count_children_timeout(console, 1, 1)
sendintr()
time.sleep(1)
proc_check.count_children_timeout(console, 1, 1)
assert os.path.exists("/proc/%d/stat" % childpid[0]), \
    'process that blocks SIGINT should not have been killed - is your shell relaying SIGINT?'

os.kill(childpid[0], signal.SIGKILL) 
time.sleep(.5)
assert not os.path.exists("/proc/%d/stat" % childpid[0]), \
    'the process was not killed'
expect_prompt("Shell did not print expected prompt (4)")

os.unlink(exe)

sendline("exit");

# ensure that no extra characters are output after exiting
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
