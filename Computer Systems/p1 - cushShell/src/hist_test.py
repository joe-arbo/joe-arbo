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

# make sure history starts with 1, shows history command
sendline('history')
expect_exact('1\thistory\r\n')
expect_prompt()

# send a few different commands
sendline('echo hi')
sendline('ls')
expect_prompt()

sendline('history')
expect_exact('1\thistory\r\n')
expect_exact('2\techo hi\r\n')
expect_exact('3\tls\r\n')
expect_exact('4\thistory\r\n')
expect_prompt()

#demonstrate up+down by navigating to top, then to 'echo hi'
sendline('\x1b[A'+'\x1b[A'+'\x1b[A'+'\x1b[A'+'\x1b[B')
expect_exact('hi\r\n')
expect_prompt()

#exit
sendline("exit")
expect("exit\r\n", "Shell output extraneous characters")

test_success()