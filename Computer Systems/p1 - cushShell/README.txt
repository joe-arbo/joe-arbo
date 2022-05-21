Student Information
-------------------
Rio Cross riocross
Joseph Arbolino joearbo13

How to execute the shell
------------------------
set your directory to 'src'
run 'make'
run './cush'

Important Notes
---------------
Closes on unsupported command

Description of Base Functionality
---------------------------------
jobs: Iterates through all jobs which exist, and calls print_job on each.
fg: Gets the job from the provided jid, then sends SIGCONT with kill() to the fetched pid from jid. Moves job to foreground and gives it the terminal.
bg: Gets the job from the provided jid, then sends SIGCONT with kill() to the fetched pid from jid. Moves job to background. 
kill: Gets the job from the provided jid, then sends SIGTERM with kill() to the fetched pid from jid.
stop: Gets the job from the provided jid, then sends SIGTSTP with kill() to the fetched pid from jid. Sets job status to STOPPED.
exit: Calls exit() with decision logic based on if a status is provided.
ˆC: The child recieves the SIGINT signal in handle_child_status, and process counter is decremented.
ˆZ: The child recieves the SIGTSTOP signal in handle_child_status, the terminal is saved, and the job status is set to STOPPED.

Description of Extended Functionality
-------------------------------------
N/A

List of Additional Builtins Implemented
---------------------------------------
Custom prompt: Built by merging strings with snprintf(). Displays username, hostname, and last token of your current directory (separated by '/').
History: Initializes history with using_history(), then stores any typed input as a command. The 'history' builtin will list the entire history, and takes no arguments. History is printed via a for loop using the number of entries, and prints out the entries indexed beginning at 1. The arrow keys work natively.
    History is tested with ../tests/stdriver.py -v custom_tests.tst
    The test is located in hist_test.py