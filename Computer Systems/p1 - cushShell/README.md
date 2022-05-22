# Project 1

### Student Information
Joseph Arbolino<br>
Rio Cross

---

### How to Execute the Shell
set your directory to 'src'<br>
run 'make'<br>
run './cush'

---

### Important Notes
Shell closes on unsupported command

---

### Description of Base Functionality
- `jobs`: Iterates through all jobs which exist, and calls print_job on each.<br>
- `fg`: Gets the job from the provided jid, then sends SIGCONT with kill() to the fetched pid from jid. Moves job to foreground and gives it the terminal.<br>
- `bg`: Gets the job from the provided jid, then sends SIGCONT with kill() to the fetched pid from jid. Moves job to background.<br>
- `kill`: Gets the job from the provided jid, then sends SIGTERM with kill() to the fetched pid from jid.<br>
- `stop`: Gets the job from the provided jid, then sends SIGTSTP with kill() to the fetched pid from jid. Sets job status to STOPPED.<br>
- `exit`: Calls exit() with decision logic based on if a status is provided.<br>
- `ˆC`: The child recieves the SIGINT signal in handle_child_status, and process counter is decremented.<br>
- `ˆZ`: The child recieves the SIGTSTOP signal in handle_child_status, the terminal is saved, and the job status is set to STOPPED.<br>

---

### List of Additional Builtins Implemented
- **Custom prompt**: Built by merging strings with snprintf(). Displays username, hostname, and last token of your current directory (separated by '/').
- **History**: Initializes history with using_history(), then stores any typed input as a command. The 'history' builtin will list the entire history, and takes no arguments. History is printed via a for loop using the number of entries, and prints out the entries indexed beginning at 1. The arrow keys work natively.
    - History is tested with ../tests/stdriver.py -v custom_tests.tst
    - The test is located in hist_test.py