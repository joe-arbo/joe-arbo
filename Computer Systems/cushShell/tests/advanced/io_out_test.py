from testutils import *
from tempfile import mkstemp

setup_tests()
   
expect_prompt()

tmpfile = '/tmp/{0}.{1}.txt'.format(int(time.time() * 1000),
                                    os.getuid())

sendline('echo hi > {0}'.format(tmpfile))
expect_prompt()

with open(tmpfile) as fd:
    data = fd.read()
    assert 'hi' in data


sendline('echo hello > {0}'.format(tmpfile))
expect_prompt()

with open(tmpfile) as fd:
    data = fd.read()
    assert 'hi' not in data
    assert 'hello' in data


os.unlink(tmpfile)
sendline('echo create this > {0}'.format(tmpfile))
expect_prompt()

with open(tmpfile) as fd:
    data = fd.read()
    assert 'hello' not in data
    assert 'create this' in data

os.unlink(tmpfile)

message = '''Test that file descriptors are not leaked into child
processes. Only the file descriptors 0, 1, and 2 (stdin, stdout, stderr)
should be open. If you don't call close() in the proper locations this
test will fail.
'''
sendline('sleep 100 > /dev/null &')
job = parse_bg_status()

assert_correct_fds(job.pid, message)

os.kill(int(job.pid), signal.SIGKILL)

test_success()
