#
# Run special ts_test program to check if shell saves/restores
# terminal state.
#
from testutils import *

exe = make_test_program(open(os.path.dirname(__file__) + "/ts_test.c").read())

setup_tests()
expect_prompt()

sendline('{0}'.format(exe))

expect('This job should now stop', 'start ts_test')
job = parse_job_line()
assert job.status == 'stopped'

run_builtin('fg', str(job.id))
expect('Job now continuing...', 'continue ts_test')

expect_prompt()

test_success()
