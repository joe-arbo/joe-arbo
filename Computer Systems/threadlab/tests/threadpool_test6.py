#!/usr/bin/env python3
#
# Run threadpool_test6 under valgrind, check for leaks
#

import sys, subprocess, re, os

valgrind_cmd = ["valgrind", "--fair-sched=yes", "--leak-check=full", "--suppressions=sigaltstack.suppression", "./threadpool_test6"]

proc = subprocess.Popen(valgrind_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
stdout, stderr = proc.communicate()
if proc.returncode != 0:
    print("test did not exit with zero", file=sys.stderr)
    sys.exit(proc.returncode)

# expect
# ==2343328== LEAK SUMMARY:
# ==2343328==    definitely lost: 0 bytes in 0 blocks
# ==2343328==    indirectly lost: 0 bytes in 0 blocks
# ==2343328==      possibly lost: 0 bytes in 0 blocks
# ==2343328==    still reachable: 0 bytes in 0 blocks
#
# or:
# ==1321353==     in use at exit: 0 bytes in 0 blocks
# All heap blocks were freed -- no leaks are possible

reports = set()
for line in stderr.decode().split('\n'):
    line = line.strip()
    m = re.match(r'==(\d+)==\s+(\S.*): (\d+) bytes in (\d+) blocks', line)
    if m:
        g = m.groups()
        cat, lost = g[1], g[2]
        if lost != '0':
            print(f'expected no leaks, but valgrind reports: {line}', file=sys.stderr)
            print(f'run with {" ".join(valgrind_cmd)}', file=sys.stderr)
            sys.exit(1)
        else:
            reports.add(cat)

# parent expects runresult.json under parent pid
try:
    os.rename(f'runresult.{os.getpid()}.json', f'runresult.{os.getppid()}.json')
except FileNotFoundError as fne:
    print (fne, file=sys.stderr)

if reports == {'possibly lost', 'indirectly lost', 'definitely lost', 'still reachable'} or reports == {'in use at exit'}:
    print ("Leak check successful.")
    sys.exit(0)
else:
    print (f'Leak check unsuccessful: {reports}', file=sys.stderr)
    sys.exit(1)
