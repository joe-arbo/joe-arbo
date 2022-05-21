#!/usr/bin/python
#
# Tests the functionality of gback's glob implement
# Also serves as example of how to write your own
# custom functionality tests.
#
import atexit, proc_check, time
from testutils import *

console = setup_tests()

# ensure that shell prints expected prompt
expect_prompt()

#################################################################
# 
# Boilerplate ends here, now write your specific test.
#
#################################################################
# Step 1. Create a temporary directory and put a few files in it
# 
#
import tempfile, shutil
tmpdir = tempfile.mkdtemp("-cush-glob-tests")
testfiles = ['aa', 'ab', 'abc', 'c']
for file in testfiles:
    open(tmpdir + "/" + file, "w")

# make sure it gets cleaned up if we exit
def cleanup():
    shutil.rmtree(tmpdir)

atexit.register(cleanup)

expectedoutput = " ".join(tmpdir + "/" + f for f in testfiles)

#################################################################
# Step 2. Run echo with a glob
#
# run echo
sendline("echo " + tmpdir + "/*");

expect_exact(expectedoutput, "expected glob expansion %s" % (expectedoutput))
# ensure that shell prints expected prompt
expect_prompt("Shell did not print expected prompt (3)")

#################################################################
# Test glob expansion at the end of the word

#################################################################
# Step 3. Run echo with a glob a*
#
sendline("echo %s/a*" % (tmpdir))

expectedoutput = " ".join(tmpdir + "/" + f for f in testfiles if f.startswith("a"))
expect_exact(expectedoutput, "echo a* does not work correctly")
expect_prompt("Shell did not print expected prompt (4)")

#################################################################
# Step 4. Run echo with a glob a?
#
sendline("echo %s/a?" % (tmpdir))

expectedoutput = " ".join(tmpdir + "/" + f for f in testfiles if f.startswith("a") and len(f) == 2)
expect_exact(expectedoutput, "echo a? does not work correctly")

test_success()
