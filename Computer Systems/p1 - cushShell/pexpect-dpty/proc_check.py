#####
#
# Collection of handy routines for testing shells
#
# DO NOT change ANY of the code.  It will break the relation between
# the tests run by you and us running your tests for grading.
# 
#
# Include this file with "import proc_check"
#
#####
import sys
import os, re, pexpect, time

def check_pid_fgpgrp(pid):
    """
    Read the /proc/<pid>/stat file and check if the process group of the pid
    is the foreground process group.  If it is the fg pgrp, return true.
    If it is not the fg pgrp, return false.  
    If there are errors opening the file, there will be an assertion error.
    """

    f = open('/proc/' + pid + '/stat', 'r')
    stat_str = f.readline()
    
    grps = re.match("\d+ \S+ . \d+ (\d+) \d+ \d+ (\d+)", stat_str)
    grps_1 = grps.group(1)
    grps_2 = grps.group(2)
    
    return grps_1 == grps_2
    f.close()

def check_pid_status(pid, pid_status):
    """
    Read the /proc/<pid>/stat file and check the one-character status of the
    process.  Compare that status to the provided pid_status.  Return true
    if the values are the same, return false if the process status and the
    pid_status differ.
    If there are errors opening the file, there will be an assertion error.
    """

    f = open('/proc/' + pid + '/stat', 'r')
    stat_str = f.readline()
    grps = re.match("\d+ \S+ (.) \d+ \d+ \d+ \d+ \d+", stat_str)
    grps_2 = grps.group(1)
    
    return grps_2 == pid_status
    f.close()
    

def count_active_children(pexpect_mod, num_children_expected):
	
	"""
	Count the number of process spawned by the provided shell that are
	currently active.  Error is asserted and the test is failed if
	the number of children expected is not at least 0, and the number of 
	children discovered is not greater than or less than the number 
	expected.
	"""
	
	assert num_children_expected >= 0, 'ERROR: invalid input'
	
	num_children_found = 0
	for pid in os.listdir("/proc/"):
		try:
			pid = int(pid)
			try:
				f = open('/proc/' + str(pid) + '/stat', 'r')
				stat_str = f.readline()
				
				#pulling the 4th number, the ppid, from the /proc/pid/stat file
				grps = re.match("\d+ \S+ . (\d+)", stat_str)
				ppid = int(grps.group(1))
				
				if ppid == pexpect_mod.pid:
					num_children_found = num_children_found + 1
			except:
					#error from failing to open the file
					#i.e. the process finished before being read
					pass
		except:
			#error from converting the filename to an integer
			#i.e. it wasn't a numbered process file
			pass

	assert num_children_found >= num_children_expected, \
			'ERROR:  Fewer children found in process than expected'
	assert num_children_found == num_children_expected, \
			'ERROR:  More children found in process than expected'

def count_children_timeout(pexpect_mod, num_children_expected, timeout = -1):
	
	"""
	Count the number of process spawned by the provided shell,
	until the timeout period ends.  Error is asserted and the test is failed if
	there is no timeout provided, the number of children expected is not at 
	least 0, and the number of children discovered is not greater than or less
	than the number expected.
	"""
	
	assert timeout is not None, \
		'Error:  Must include timeout or allow default timeout'
	assert num_children_expected >= 0, \
		'ERROR: input for num_children_expected invalid'
	
	#The default timeout is the timeout of the pexpect module
	if timeout == -1:
		timeout = pexpect_mod.timeout
	end_time = time.time() + timeout
	
	num_children_found = 0
	children_found = []
	
	#timeout loop, always runs for timeout seconds
	while timeout > 0:
		#minor sleep, to lessen busy-waiting processing
		time.sleep(0.010)
		for pid in os.listdir("/proc/"):
			try:
				pid = int(pid)
				try:
					f = open('/proc/' + str(pid) + '/stat', 'r')
					stat_str = f.readline()
					
					#pulling the first and fourth numbers, the pid and ppid
					grps = re.match("(\d+) \S+ . (\d+)", stat_str)
					pid = int(grps.group(1))
					ppid = int(grps.group(2))
					
					if ppid == pexpect_mod.pid and children_found.count(pid) == 0:
						num_children_found = num_children_found + 1
						children_found.append(pid)
				except:
					#error from failing to open the file
					#i.e. the process finished before being read
					pass
			except:
				#error from converting the filename to an integer
				#i.e. it wasn't a numbered process file
				pass
		
		timeout = end_time - time.time()

	assert num_children_found >= num_children_expected, \
			'ERROR:  Fewer children found in process than expected'
	assert num_children_found == num_children_expected, \
			'ERROR:  More children found in process than expected'
        return children_found

def wait_until_child_is_in_foreground(pexpect_mod, timeout = -1):

    """
    This begins a timeout loop that waits for a process id other than that
    of the shell (self.pid) to take over the pty.  A timeout is raised if the
    terminal is not taken over, and the while loop quits if the terminal is
    taken over.  There is a 10ms sleep to slow the pace of the busy-waiting.
    There is a chance that a program could start and finish in those said 10ms,
    and as such tests should be designed to not wait for extremely fast 
    processing commands.
    """
    
    #the default timeout is the timeout from the pexpect module
    if timeout == -1:
	    timeout = pexpect_mod.timeout
    if timeout is not None:
        end_time = time.time() + pexpect_mod.timeout 

	#busy wait with a sleep until the pty is taken over by a process
	while os.tcgetpgrp(pexpect_mod.child_fd) == pexpect_mod.pid:
		time.sleep(0.010)            
		if timeout is not None:
			timeout = end_time - time.time()
                    
		if timeout < 0 and timeout is not None:
			raise pexpect.TIMEOUT ('Timeout exceeded in \
									wait_until_child_is_in_foreground().')

	process_pid = os.tcgetpgrp(pexpect_mod.child_fd)
	assert check_foreground_process(pexpect_mod.pid, process_pid), \
			'Error, terminal not taken over by child process properly'

def check_foreground_process(parent_pid, process_pgrp):

	"""
	Check the foreground process identified by 
		wait_until_child_is_in_foreground that it:
	1.  Owns the tty (implicit from how the process is identified)
	2.  Is a child of the shell.
	"""

	if os.path.exists('/proc/' + str(process_pgrp) + '/stat'):
		try:
			# as per http://man7.org/linux/man-pages/man5/proc.5.html
			# pid (exename) state ppid pgrp session tty_nr tpgid
			f = open('/proc/' + str(process_pgrp) + '/stat', 'r')
			stat_str = f.readline()
			grps = re.match("\d+ \S+ . (\d+) (\d+) \d+ \d+ (\d+)", stat_str)
			ppid = grps.group(1)	# ppid
			pgrp = grps.group(2)	# pgrp
			tpgid = grps.group(3)	# tpgid

			if pgrp != tpgid:
				print "pid %d (ppid %s, pgrp %s) does not own tty. It's owned by %s" % (process_pgrp, ppid, pgrp, tpgid)

			assert pgrp == tpgid, 'Error, the process does not own the tty'

			assert int(ppid) == parent_pid, \
					'Error, process is not a child of the shell'

			f.close()
			return True
		except IOError, e:
			print "Error opening the proc/<pid>/stat file for the process.  \
					Advise: attempt re-run."
			return False
	
	else:
		print "Error: The proc/<pid>/stat file for the process does not exist.\
		  Run a command that won't have the process group leader quit early."
		return False
