from testutils import *
from tempfile import mkstemp

setup_tests()


_, tmpfile = mkstemp()

expect_prompt()

message = '''Simple IO redirect append test.
Makes sure your program actually creates the file, not just
appending to existing.

echo b >> tmpfile
echo a >> tmpfile
'''

sendline('echo first line >> {0}'.format(tmpfile))
expect_prompt()

with open(tmpfile) as fd:
    data = fd.read()
    assert 'first line' == data.strip()


sendline('echo second line >> {0}'.format(tmpfile))
expect_prompt()

with open(tmpfile) as fd:
    data = fd.read()
    assert 'first line\nsecond line' == data.strip()

os.unlink(tmpfile)

sendline('echo new line >> {0}'.format(tmpfile))
expect_prompt()

with open(tmpfile) as fd:
    data = fd.read()
    assert 'new line' == data.strip()

os.unlink(tmpfile)

with open(tmpfile, 'w') as fd:
    fd.write('now I create the data\n')

sendline('echo and you append >> {0}'.format(tmpfile))
expect_prompt()
with open(tmpfile) as fd:
    assert 'now I create the data\nand you append' == fd.read().strip()

#
# test that the shell can redirect both stdout and stderr
#
os.unlink(tmpfile)
exe = make_test_program(open(os.path.dirname(__file__) + "/writetostderr.c").read())
sendline('{0} >& {1}'.format(exe, tmpfile))
expect_prompt()
os.unlink(exe)

with open(tmpfile) as fd:
    content = fd.read()
    assert 'stderr\nstdout\n' == content

os.unlink(tmpfile)

test_success()
