#!/usr/bin/python
#
# Tests that the shell can run commands in the foreground
# This is the very first test you should pass
# 

import sys, imp, atexit, pexpect, proc_check, signal, time
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

# run a command
sendline("echo Hello cush")
expect_exact("Hello cush\r\n", "Shell could not run echo")

# run a command
sendline("/usr/bin/gcc")

# separate to work around diagnostics colors that are printed by default
expect("gcc", "Shell did not start gcc")
expect("fatal error", "gcc did not print fatal error")
expect("no input files", "gcc did not print about no input files")

# make sure shell returns to prompt
expect_prompt("Shell did not print expected prompt (2)")

# the cush shell should respect the PATH variable, and /usr/bin is in the path
# test whether adding a single command line argument works
sendline("gcc -fno-diagnostics-color")

expect("gcc: fatal error: no input files", "Shell did not start gcc from path")

expect_prompt("Shell did not print expected prompt (3)")

# send EOF
console.sendeof()

test_success()
