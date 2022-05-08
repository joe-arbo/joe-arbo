#
# test various combinations involving multiple pipelines on one
# command line
#
import os
from tempfile import mkstemp
from testutils import *

setup_tests()

expect_prompt()

_, tmpfile = mkstemp()
os.close(_)

sendline('echo hi | cat > {0}; < {1} rev | cat | rev | rev'.format(tmpfile, tmpfile))

expect_exact("ih\r\n", "multiple I/O + cats + revs didn't work")
expect_prompt("Shell did not print expected prompt (2)")

sendline('sh -c "echo $#" a b | grep -c 1 > {0}; cat < {1}'.format(tmpfile, tmpfile))
expect_exact("1\r\n", "double quotes do not work")
expect_prompt("Shell did not print expected prompt (3)")

sendline('sh -c "echo $#" a b c | grep -c 2 > {0}; cat < {1}'.format(tmpfile, tmpfile))
expect_exact("1\r\n", "double quotes do not work")
expect_prompt("Shell did not print expected prompt (4)")

sendline('sh -c "echo $#" a "b c" | grep -c 1 > {0}; cat < {1}'.format(tmpfile, tmpfile))
expect_exact("1\r\n", "double quotes do not work")
expect_prompt("Shell did not print expected prompt (5)")

sendline('rm {0}; touch {0}; echo "hi" > {0}; rev < {0}'.format(tmpfile, tmpfile, tmpfile, tmpfile))
expect_exact("ih\r\n", "I/O redirect files aren't created in order")
expect_prompt("Shell did not print expected prompt (6)")

os.unlink(tmpfile)

test_success()
