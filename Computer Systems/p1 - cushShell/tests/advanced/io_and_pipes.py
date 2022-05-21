from testutils import *
from tempfile import mkstemp

setup_tests()
   
expect_prompt()

_, tmpfile = mkstemp()

with open(tmpfile, 'w') as fd:
    fd.write('123 456 789 10\n')

message = '''Setup a multiple pipeline from an input file'''
sendline('sed s/123/hello/ < {0} | sed s/456/world/ | rev'.format(tmpfile))
expect('01 987 dlrow olleh', message)
expect_prompt()

os.unlink(tmpfile)

# the following command should (a) complete and (b) output 333
# this test tends to fail if the parent retains pipe file descriptors
# because 'wc' then doesn't exit when head exits and closes the pipe
sendline('yes test | head -n 333 | wc -l')
expect("333")
expect_prompt()

# now repeat this combined with I/O redirection ending the pipeline
tmpfile = '/tmp/{0}.{1}.txt'.format(int(time.time() * 1000),
                                    os.getuid())

sendline('yes test | head -n 3 | wc -l > {0}'.format(tmpfile))
expect_prompt()
with open(tmpfile) as fd:
    assert '3' == fd.read().strip()

os.unlink(tmpfile)

expected_words = 'this is a bunch of words words'.split()
with open(tmpfile, 'w') as fd:
    for word in expected_words*2:
        fd.write(word + '\n')


_, tmpout = mkstemp()
sendline('sort < {0} | uniq | rev > {1}'.format(tmpfile, tmpout))
expect_prompt()

with open(tmpout) as fd:
    lines = map(lambda x: x.strip(), fd.readlines())
    assert lines == 'a hcnub si fo siht sdrow'.split()

os.unlink(tmpfile)
os.unlink(tmpout)

_, tmpfile = mkstemp()
sendline('yes test | head -n 100000 > {0}'.format(tmpfile))
expect_prompt()

_, tmpout = mkstemp()
sendline('wc -l < {0}  | rev > {1}'.format(tmpfile, tmpout))
expect_prompt()

with open(tmpout) as fd:
    assert fd.read().strip() == '000001'

os.unlink(tmpfile)
os.unlink(tmpout)

test_success()

