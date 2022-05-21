from testutils import *
from tempfile import mkstemp

setup_tests()
    
_, tmpfile = mkstemp()
testdata = 'this is a simple test'
with open(tmpfile, 'w') as fd:
    fd.write(testdata)

message = '''Simple IO redirect input test.
Sets up a tmp file and writes to it, makes sure your output matches.
cat < tmpfile
rev < tmpfile

Overwrite file
wc -c < tmpfile
'''

sendline('cat < {0}'.format(tmpfile))
expect(testdata, message)

sendline('rev < {0}'.format(tmpfile))
expect('tset elpmis a si siht', message)

with open(tmpfile) as fd:
    assert fd.read() == 'this is a simple test'

with open(tmpfile, 'w') as fd:
    fd.write('overwrite')

sendline('wc < {0}'.format(tmpfile))
expect(tmpfile)
expect_regex(r'(0)\s+(1)\s+(9)')


os.unlink(tmpfile)

message = '''Test that file descriptors are not leaked into child
processes. Only the file descriptors 0, 1, and 2 (stdin, stdout, stderr)
should be open. If you don't call close() in the proper locations this
test will fail.
'''
sendline('sleep 100 < /dev/null &')
job = parse_bg_status()

assert_correct_fds(job.pid, message)

os.kill(int(job.pid), signal.SIGKILL)

test_success()
