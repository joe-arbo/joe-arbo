from testutils import *

setup_tests()

    
message = '''Test very simple pipe application:
echo hello | rev'''

sendline('echo hello | rev')
expect('olleh', message)

message = '''Send some words through sed for correct subs:
echo hello apple | sed s/apple/wormhole/'''

sendline('echo hello apple | sed s/apple/wormhole/')
expect('hello wormhole', message)

message = '''Test that programs are actually ran at the same time
Pipes are buffered, but only to a certain point (64K on Linux)
so if no program consumes the data, after 64K the program will
block on write() because the pipe is full.

If your shell does not start programs simultaneously
(eg: you fork() -> wait() -> fork() -> wait()) this test
will fail.
'''

sendline('head -c 1000000 /dev/urandom | wc -c')
expect('1000000', message)
expect_prompt(message)

message = '''Test that file descriptors are not leaked into child
processes. Only the file descriptors 0, 1, and 2 (stdin, stdout, stderr)
should be open. If you don't call close() in the proper locations this
test will fail.
'''
sendline('sleep 100 | sleep 100 &')
job = parse_bg_status()

assert_correct_fds(job.pid, message)
expect_prompt()

os.killpg(os.getpgid(int(job.pid)), signal.SIGKILL)

test_success()
