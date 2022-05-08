"""
Test for 'exclusive access' of the terminal by
playing with vim and nano.

Tests simply opening and typing,
then some background and job control with exclusive access
to make sure you maintain proper terminal state.
"""

from testutils import *
from tempfile import mkstemp
setup_tests()


def expect_stopped(command):
    run_builtin('jobs')

    job = parse_job_line()

    assert command in job.command, 'Job was not: {0}'.format(command)
    assert 'stopped' in job.status.lower(), 'Vim not stopped'
    expect_prompt()
    return job

expect_prompt()
_, tmpfile = mkstemp()
# Start nano
sendline('nano {0}'.format(tmpfile))
wait_for_fg_child()

# Quit out to make sure nano can read keys
sendline('test that it works')
time.sleep(0.5)
sendcontrol('x')
time.sleep(0.5)
sendline('y\r')
expect_prompt()

with open(tmpfile) as fd:
    assert 'test that it works' in fd.read()
os.unlink(tmpfile)


# Assert that the shell has regained control


# Try to start vim in the background
# Should stop and wait for exclusive control
sendline('vim -u NONE &')
parse_bg_status()
expect_prompt()
time.sleep(0.5)
job = expect_stopped('vim')
assert job.id == '1', 'There should be no other jobs running so vim should get job id 1'

# Send vim that was stopped into the foreground
run_builtin('fg', job.id)
wait_for_fg_child()


# Stop the currently running vim
sendcontrol('z')
expect_prompt()

job = expect_stopped('vim')
assert job.id == '1', 'There should be no other jobs running so vim should get job id 1'

# Send it back into the foreground and try and quit it.
run_builtin('fg', job.id)

wait_for_fg_child()

sendline(':q')

# See if shell correctly regains control
expect_prompt()

test_success()

