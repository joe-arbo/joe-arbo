from testutils import *
from tempfile import mkstemp

setup_tests()

expect_prompt()
    
message = '''Test very simple pipe application:
echo hello |& rev'''

sendline('echo hello |& rev')
expect('olleh', message)

expect_prompt()

_, tmpfile = mkstemp()

exe = make_test_program(open(os.path.dirname(__file__) + "/writetostderr.c").read())
sendline('{0} |& cat > {1}'.format(exe, tmpfile))
expect_prompt()
os.unlink(exe)

with open(tmpfile) as fd:
    content = fd.read()
    assert 'stderr\nstdout\n' == content

os.unlink(tmpfile)

test_success()
