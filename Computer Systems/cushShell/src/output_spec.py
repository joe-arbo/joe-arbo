#
# This file contains definitions that describe the output of your shell.
#
# You may adapt all settings in this file to match the output of your
# shell. (Alternatively, you may write your shell to match these templates.)
#

# the shell executable.
shell = "./cush"

# the prompt printed by your shell, as a regular expression
# prompt = "cush>"
prompt = "<[^@]*@[^>]*>\$ " #our prompt fits gback's

# change your prompt once you have customized your shell.
# Here's an example
import getpass
if getpass.getuser() == "gback":
    prompt = "<[^@]*@[^>]*>\$ "

#
# a regexp matching the message printed when a job is sent into the background
# must capture (jobid, processid)
#
bgjob_regex = "\[(\d+)\] (\d+)"

#
# a regexp matching a job status when printed using the 'jobs' command
# must capture (jobid, jobstatus, commandline)
#
job_status_regex =  "\[(\d+)\].?\s+(\S+)\s+\((.+?)\)\r\n"

#
# job status messages your shell prints
#
jobs_status_msg = {
    'stopped' : "Stopped",
    'running' : "Running"
}

#
# builtin commands
#
# Use printf-style formats. stop, kill, fg, and bg expect job ids.
# If your shell requires a % before the jobid, use %%%s.
#
builtin_commands = {
    'jobs' : 'jobs',
    'stop' : 'stop %s',
    'kill' : 'kill %s',
    'fg'   : 'fg %s',
    'bg'   : 'bg %s',
    'exit' : 'exit'
}

# Uncomment this line if you like stdriver.py to keep a log of everything
# input and output to the pty in file 'log.txt'
#
logfile=open("log.txt", "w")
