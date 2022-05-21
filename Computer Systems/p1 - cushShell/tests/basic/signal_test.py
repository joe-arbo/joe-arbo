#
# check that shell prints standard messages for children killed by signals
#
import sys, imp, atexit, pexpect, proc_check, signal, time, threading
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

exe = make_test_program(open(os.path.dirname(__file__) + "/die.c").read())

sendline('{0} -divzero'.format(exe))
console.ignorecase = True
expect('floating point exception')

sendline('{0} -segfault'.format(exe))
expect('segmentation fault')

sendline('{0} -abort'.format(exe))
expect('aborted')

sendline('{0}'.format(exe))
childpid = proc_check.count_children_timeout(console, 1, 1)
os.kill(childpid[0], signal.SIGKILL) 
expect('killed')

sendline('{0}'.format(exe))
childpid = proc_check.count_children_timeout(console, 1, 1)
os.kill(childpid[0], signal.SIGTERM) 
expect('terminated')

os.unlink(exe)

sendline("exit");

# ensure that no extra characters are output after exiting
expect_exact("exit\r\n", "Shell output extraneous characters")

test_success()
