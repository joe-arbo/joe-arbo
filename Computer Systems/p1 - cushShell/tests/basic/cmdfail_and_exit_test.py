#!/usr/bin/python
#
# cmdfail-test: tests that the shell properly terminates any forked
# children and returns to the prompt if the user types in a command
# that is neither a built-in nor a Unix command.
# 
# Requires the following builtin commands to be implemented
# or otherwise usable:
#
#	exit
#

#!/usr/bin/python
#
# Tests that the shell can run commands in the foreground
# This is the very first test you should pass
# 

import sys, imp, atexit, pexpect, proc_check, signal, time, os
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a non-existent command
console.sendline("this_command_does_not_exist")

time.sleep(1)
# this should fail somehow and not leave any children behind
proc_check.count_active_children(console, 0)

expect_prompt("Shell did not print expected prompt")

# end the shell program  with exit
run_builtin('exit')

# this will kill the shell if still alive after some delay
console.close()
# but it should have exited by itself if exit was implemented
assert os.WIFEXITED(console.status)
assert os.WEXITSTATUS(console.status) == 0

test_success()
