#####
#
# Collection of handy routines for testing shells
#
# Include this file with "import shelltests"
#
#####

import sys, imp, atexit
import os, re, time, pexpect, tempfile, proc_check, shutil, stat, signal, traceback
from collections import namedtuple

def test_success(msg = "", exit = True):
    """Print PASS message and optionally 'msg'.  Exit unless 'exit is False"""
    print "PASS", msg
    if exit:
        sys.exit(0)

def parse_regular_expression(pexpect_child, regex):
    """
    A regular expression was printed and is matched into groups and returned.
    The return value will be a tuple of all of the parenthetical groups found
    in the regular expression.  For example:
    regex = \[(\d+)\] (\d+) will return a tuple
    of the form (decimal_one, decimal_two) as it was read from the output
    of the shell "[decimal_one] decimal_two".
    """
    pexpect_child.expect(regex)
    return __regex2tuple(pexpect_child.match)

def __regex2tuple(match):
    """Turn a matched regular expression into a tuple of its captured groups"""
    return tuple([ match.group(i) for i in range(1, match.lastindex + 1) ])

def handle_exception(type, exc, tb):
    """Install a default exception handler.
    If there is a pexpect.TIMEOUT exception thrown at any time in the script,
    report that the test failed because of a timeout and exit.
    """
    if type == pexpect.TIMEOUT:
        print "\n>>> FAIL: Test timed out", exc.get_trace(), "\n"
    else:
        print "\n>>> FAIL: ", type, "'", exc, "'\n"
    traceback.print_tb(tb)

sys.excepthook = handle_exception

console = None
settings_module = None

def setup_tests(additional_cmdline_arguments = []):
    global console
    global settings_module
    
    definitions_scriptname = sys.argv[1]
    settings_module = imp.load_source('', definitions_scriptname)
    logfile = None
    if hasattr(settings_module, 'logfile'):
        logfile = settings_module.logfile

    #spawn an instance of the shell
    console = pexpect.spawn(settings_module.shell + " ".join(map(str, additional_cmdline_arguments)), drainpty=True, logfile=logfile or sys.stdout)
    atexit.register(kill, shell_process=console)

    # set timeout for all following 'expect*' calls to 2 seconds
    console.timeout = 2 
    return console


def sendline(line):
    console.sendline(line)

def sendcontrol(ch):
    console.sendcontrol(ch)

def sendintr():
    console.sendintr()

def expect(line, message=None):
    if message is None:
        message = 'expected "{}" but did not find'

    assert console.expect(line) == 0, message

def expect_exact(line, message=None):
    if message is None:
        message = 'expected "{}" but did not find'

    assert console.expect_exact(line) == 0, message

def expect_prompt(message=None):
    if message is None:
        message = 'shell did not return to prompt'

    assert console.expect(settings_module.prompt) == 0, message

def expect_regex(regex):
    return parse_regular_expression(console, regex)

def kill(shell_process):
    console.close(force=True)

def wait_for_fg_child():
    proc_check.wait_until_child_is_in_foreground(console)

def parse_job_line():
    job_status = namedtuple('job_status', ['id', 'status', 'command'])

    jid, status, cmd = expect_regex(settings_module.job_status_regex)
    for name, val in settings_module.jobs_status_msg.items():
        if val == status:
            status = name
            break
    
    return job_status(jid, status, cmd)

def parse_bg_status():
    bg_status = namedtuple('bg_status', ['job_id', 'pid'])
    jid, pid = expect_regex(settings_module.bgjob_regex)
    return bg_status(jid, pid)

def run_builtin(command, *args):
    command = settings_module.builtin_commands.get(command, command)
    sendline(command % tuple(args))

def assert_correct_fds(pid, message):
    '''Checks that file descriptors are not leaked into
    the child. 
    '''

    time.sleep(0.5)
    fds = sorted(os.listdir('/proc/{0}/fd'.format(pid)))

    if not (fds == list('012')):
        raise Exception('File descriptors leaked into child! Remember to close() all of the pipes and IO redir file descriptors')

def get_shell_pid():
    return console.pid

def make_test_program(src):
    exefile, exefilename = tempfile.mkstemp()
    os.close(exefile)
    ofile, ofilename = tempfile.mkstemp(suffix=".c")
    os.write(ofile, src)
    os.close(ofile)
    os.system("gcc %s -o %s" % (ofilename, exefilename))
    os.unlink(ofilename)
    return exefilename

