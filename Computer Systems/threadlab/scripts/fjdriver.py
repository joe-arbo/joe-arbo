#!/usr/bin/python3

#
# fjdriver.py for Fork/Join pool projects
#
# Written by Godmar Back and Scott Pruett for CS 3214
#
# https://git-scm.com/docs/pretty-formats
version = "$Format:%H committed by %cn$"

#
import getopt, sys, os, subprocess, signal, re, json, resource, time, socket
from datetime import datetime
from collections import namedtuple, defaultdict

# add location of this script to python sys.path
# add directory in which script is located to python path
script_dir = "/".join(__file__.split("/")[:-1])
if script_dir == "":
    script_dir = "."

script_dir = os.path.realpath(script_dir)
if script_dir not in sys.path:
    sys.path.append(script_dir)

from fjtests import load_test_module

src_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + "/tests"
results_file = "full-results.json"
filelist = src_dir + "/FILELIST"
timestamp = str(datetime.now()).replace(" ", "_")
workdir = "fj_testdir_" + timestamp
poolfile = "./threadpool.c"
verbose = False
silent = False
list_tests = False
grade_mode = False
benchmark_runs = 1

# Benchmark info

# Times of serial runs for benchmarked tests
# XXX these are out of date currently and ultimately not used.
benchmark_times = {
    'nqueens 14':      17.158,
    'quicksort large': 19.81442,
    'mergesort large': 22.33417
}

tests = load_test_module('standard')

#
# getopt
#
# look for threadpool.c in current dir, or point at with flag
#
def usage():
    print ("""
Usage: %s [options]
    -v              Verbose
    -V              Print Version and exit
    -a              Run benchmark anyway even if machine is not idle
    -r              Only run required tests.
    -h              Show help 
    -p              <file> - Location of threadpool implementation, default ./threadpool.c
    -l              List available tests
    -t              Filter test by name, given as a comma separated list.
                    e.g.: -t basic1,psum
    """ % (sys.argv[0]))

try:
    opts, args = getopt.getopt(sys.argv[1:], "Varvhlp:t:o:B:gL", ["verbose", "help", "list-tests"])
except getopt.GetoptError as err:
    print (str(err)) # will print something like "option -a not recognized"
    usage()
    sys.exit(2)

runfilter = lambda test : True
ignore_if_not_idle = False

for opt, arg in opts: 
    if opt == "-r":
        oldrunfilter = runfilter
        runfilter = lambda test: test.is_required and oldrunfilter(test)

    elif opt == "-V":
        print ("Version", version)
        sys.exit(0)
    elif opt == "-a":
        ignore_if_not_idle = True
    elif opt == "-v":
        verbose = True
    elif opt in ("-h", "--help"):
        usage()
        sys.exit()
    elif opt == '-p':
        poolfile = arg
    elif opt == '-l':
        list_tests = True
    elif opt == '-o':
        results_file = arg
    elif opt == '-B':
        benchmark_runs = int(arg)
    elif opt == '-g':
        grade_mode = True
    elif opt == '-t':
        filtered = arg.split(',')
        for _filter in filtered:
            for test in tests:
                if _filter == test.name:
                    break
            else:
                print ('Unknown test: %s. Use -l to list test names.' % _filter)
                usage()
                sys.exit()
        oldrunfilter = runfilter
        runfilter = lambda test: test.name in filtered and oldrunfilter(test)
    else:
        assert False, "unhandled option"

if list_tests:
    print ('Available tests (with applied filters):')
    print (80 * '=')
    for test in tests:
        if runfilter(test):
            print ('%s: %s' % (test.name, test.description))
    sys.exit()

def copyfile(src, dst):
    cmd = "cp %s %s" % (src, dst)
    if verbose:
        print (cmd)
    ex = os.system(cmd)
    if ex:
        sys.exit(ex)

def setup_working_directory():
    if verbose:
        print ("Creating working directory",  workdir)

    os.mkdir(workdir)

    if verbose:
        print ("Copying files")

    if not os.access(poolfile, os.R_OK):
        print ()
        print ("I cannot find %s" % poolfile)
        usage()
        sys.exit(2)

    copyfile(poolfile, workdir + "/threadpool.c")

    flist = open(filelist, 'r')
    for file in flist:
        if file.startswith("#"):
            continue
        file = file.strip()
        copyfile(src_dir + "/" + file, workdir)

    flist.close()
    if verbose:
        print ("Copying %s" % poolfile)

    os.chdir(workdir)

    if os.system("make -j 20"):
        if grade_mode:
            op = open(results_file, 'w')
            op.write(json.dumps({'error': 'did not compile'}))
            op.close()
        raise Exception("make failed, run 'make' in %s to see why" % workdir)

def check_software_engineering(objfile, allowedsymbols):
    hex = "[0-9A-Fa-f]{8,16}"
    if verbose:
        print ("Performing some checks that %s conforms to accepted software engineering practice..." % objfile)

    symbols = subprocess.Popen(["nm", objfile], stdout=subprocess.PIPE)\
        .communicate()[0].decode().split("\n")

    for sym in symbols:
        if sym == "" or re.match("\s+U (\S+)", sym):
            continue

        m = re.match(hex + " (\S) (\S+)", sym)
        if not m:
            raise Exception("unexpected line in nm:\n" + sym)

        if m.group(1).islower():    # local symbols are fine
            continue

        if m.group(1) == "T":
            if m.group(2) in allowedsymbols:
                continue
            
            if grade_mode:
                op = open(results_file, 'w')
                op.write(json.dumps({'error': 'defines global function %s' % m.group(2)}))
                op.close()
            raise Exception(("%s defines global function '%s'\n"
                +"allowed functions are: %s") % (objfile, m.group(2), str(allowedsymbols)))

        if grade_mode:
            op = open(results_file, 'w')
            op.write(json.dumps({'error': 'defines global symbol %s' % m.group(2)}))
            op.close()
        raise Exception(("%s must not define any global or static variables"
                +", but you define: %s") % (objfile, m.group(2)))

allowedsymbols = [ "future_free", "future_get",
                   "thread_pool_new", "thread_pool_shutdown_and_destroy", 
                   "thread_pool_submit" ] 

#
# build it (like check.py)
#

def count_number_of_processes():
    proc = subprocess.Popen(["ps", "ux", "-L"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    stdout, stderr = proc.communicate()
    # -2 for ps and header
    return len(stdout.decode().strip().split('\n')) - 2

def get_load_average():
    """
    Returns tuple nproc, loadavg where nproc is the current number of
    running threads (minus 1) and loadavg is the load average
    """
    # 0.57 0.65 0.54 1/426 28121
    with open("/proc/loadavg") as f:
        c = f.read().strip()
        m = re.match(r'(\S+) \S+ \S+ (\d+)/\d+ \d+', c)
        load = float(m.groups()[0])
        nprocs = int(m.groups()[1])

    return nprocs - 1, load

def wait_for_load_to_go_down():
    while True:
        time.sleep(0.2)
        nprocs, load = get_load_average()
        if nprocs == 0 and load < 1.0:
            break

        print ("Warning. There are other %d processes running on this machine, loadavg %f." % (nprocs, load))
        print ("Sleeping for 1 second.  Use the -a switch to run the benchmarks regardless.")
        time.sleep(0.8)

# run tests
#
# echo test:
#  progname  + set of inputs/cmdline parameters
#
# each should support -n <thread> for number of threads
#

#
# result: expected output + exit code(?)
#

def set_threadlimit(nthreads):
    def limit_threads():
        resource.setrlimit(resource.RLIMIT_NPROC, (nthreads, nthreads))
    return limit_threads

def run_single_test(test, run, threads):
    cmdline = ['timeout', str(run.timeout), test.command, '-n', str(threads)] + run.args
    rundata = {
        'command' : ' '.join(cmdline),
        'nthreads' : threads
    }
    def addrundata(d):
        for k, v in d.items():
            rundata[k] = v

    if not silent:
        print ('Running:', ' '.join(cmdline), end=' ')
        sys.stdout.flush()
    infile = None
    if run.input_file:
        infile = open(run.input_file, 'r')
    # preexec_fn sets the system-wide NPROC for this user.
    # we set it to #threads + 1 (for the main thread)
    # plus existing procs
    starttime = time.time()
    preexec_fn=set_threadlimit(threads + 2 + number_of_existing_processes) if test.limit_threads \
                else (lambda : None)
    proc = subprocess.Popen(cmdline, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE, stdin=infile,
                            preexec_fn=preexec_fn)

    stdout, stderr = proc.communicate()
    if grade_mode:
        stdout = stdout[:100]
        stderr = stderr[:100]
    runningtime = time.time() - starttime

    if infile:
        infile.close()

    signames = dict((k, v) for v, k in signal.__dict__.items() if v.startswith('SIG'))
    signum = proc.returncode - 128
    if proc.returncode < 0:
        signum = -proc.returncode
    if proc.returncode >= 128 or proc.returncode < 0 and signum in signames:
        timeoutmsg = ''
        if runningtime >= run.timeout:
            timeoutmsg = '\nProgram ran and most likely timed out at %.3fs' % (runningtime)
        error = """\
        Program terminated with signal %d (%s) %s
        --------------------------------------------
        Program output:
        %s
        StdErr output:
        %s
        """ % (signum, signames[signum], timeoutmsg, stdout.decode(), stderr.decode())
        addrundata({
            'error': error
        })
        if not silent:
            print ('[ ]')
            if verbose or grade_mode:
                print (error)

    elif proc.returncode > 0:
        # non-zero exit code
        timeoutmsg = ''
        if proc.returncode == 124:
            timeoutmsg = '\nProgram ran and most likely timed out at %.3fs' % (runningtime)

        error = """\
        Program exited with non-zero exit status %d. %s
        --------------------------------------------
        Program output:
        %s
        StdErr output:
        %s
        """ % (proc.returncode, timeoutmsg, stdout.decode(), stderr.decode())

        addrundata({
            'error': error
        })
        if not silent:
            print ('[ ]')
            if verbose or grade_mode:
                print (error)

    else:
        if not silent:
            print ('[+]')

        outfile = 'runresult.%d.json' % (proc.pid)
        addrundata({'stdout': stdout.decode()})
        if len(stderr) > 0: 
            addrundata({'stderr': stderr.decode()})

        if not os.access(outfile, os.R_OK):
            addrundata({
                'error': 'The benchmark did not create the expected result file %s' % outfile
            })
        else:
            with open(outfile, 'r') as f:
                data = f.read()

            addrundata(json.loads(data))
            os.unlink(outfile)
    return rundata
    
def average_run(runs):
    data = {
        'nthreads': runs[0]['nthreads'],
        'command': runs[0]['command']
    }
    totalrtime = 0.0
    totalstime = 0.0
    totalutime = 0.0
    error = None
    passed = 0
    for run in runs:
        if 'error' in run:
            error = run['error']
        else:
            passed += 1
            totalrtime += run['realtime']
            totalstime += run['ru_stime']
            totalutime += run['ru_utime']

    if error or passed != len(runs):
        data['error'] = error
        data['run_count'] = len(runs)
        data['passed'] = passed
    else:
        data['realtime'] = totalrtime / len(runs)
        data['cputime'] = (totalstime + totalutime) / len(runs)
        data['ru_stime'] = totalstime / len(runs)
        data['ru_utime'] = totalutime / len(runs)
        data['run_count'] = len(runs)
        data['passed'] = passed

    return data

def benchmark_speedup(data, testname):
    if 'realtime' not in data or 'cputime' not in data:
        return
    serial_time = benchmark_times[testname]
    data['speedup'] = serial_time / data['realtime']
    data['cpu_overuse'] = data['cputime'] / data['realtime']

def run_tests(tests):
    results = defaultdict(dict)

    summary = {}
    for test in tests:
        if not runfilter(test):
            if verbose:
                print ('Skipping test: ' + test.description)
            continue

        if not silent:
            print ('')
            print ('Starting test: ' + test.description)
            print ('=' * 80)

        results[test.name] = {}
        for run in test.runs:
            perthreadresults = []
            results[test.name][run.name] = perthreadresults

            for threads in run.thread_count:
                if grade_mode:
                    repeats = benchmark_runs if test.is_required or run.is_benchmarked else 1
                    runs = []
                    for repeat in range(repeats):
                        runs.append(run_single_test(test, run, threads))
                    rundata = average_run(runs)
                    rundata['runs'] = runs
                    if run.is_benchmarked:
                        benchmark_speedup(rundata, run.name)
                    perthreadresults.append(rundata)
                else:
                    runs = [run_single_test(test, run, threads)]
                    rundata = average_run(runs)
                    rundata['runs'] = runs
                    perthreadresults.append(rundata)
    return results

def print_results(results):
    print (json.dumps(results, indent = 4, sort_keys = True, separators = (',', ': ')))
    # try pretting printing errors for better readability
    try:
        def print_info(o):
            for k in ['error', 'stderr', 'stdout']:
                if k in o:
                    print (o[k])

        for test, r in results.items():
            for rr in r.values():
                for o in rr:
                    print_info(o)
                    if 'runs' in o:
                        map(print_info, o['runs'])

    except Exception as e:
        print(e)
        pass


def write_results_to_json(filename):
    jfile = open(results_file, "w")
    print (json.dumps(results, indent = 4, sort_keys = True, separators = (',', ': ')), file=jfile)
    jfile.close()

def find_thread_run(perthreadresults, threadcount):
    return next(filter(lambda result: result['nthreads'] == threadcount, perthreadresults), None)

def print_grade_table(results, tests):
    # report the results of running each tests with 
    thread_headers = [1, 2, 4, 8, 16, 32]
    print ('')
    print ('Test name:' + (16 * ' ') + ''.join(map(lambda x: '%-10s' % str(x), thread_headers)))
    print ('='*80)
    minimum_requirements = True
    for test in tests:
        if not runfilter(test) and test.is_required:
            if not silent:
                print ('WARNING: Skipping minimum requirement test (%s), will not say you passed!' % test.name)
            minimum_requirements = False

        if not runfilter(test):
            continue
        if not test.name in results:
            print ('%s: could not find test data!' % test.name)
        res = results[test.name]
        print ('%s:' % test.name.upper() + '  ' + test.description)
    
        passed = True
        for run in test.runs:
            statuses = []
            for threads in thread_headers:
                thread_run = find_thread_run(res[run.name], threads)
                if not thread_run:
                    statuses.append('')  # MISSING
                elif 'error' in thread_run:
                    passed = False
                    statuses.append('[ ]')
                elif run.is_benchmarked:
                    statuses.append('[%.3fs]' % thread_run['realtime'])
                else:
                    statuses.append('[X]')

            print ('  %-23s' % (run.name) + ''.join(map(lambda x: '%-10s' % x, statuses)))
        
        if not passed and test.is_required:
            minimum_requirements = False

    print ('='*80)
    if minimum_requirements:
        print ('You have met minimum requirements, your performance score will count.')
    else:
        print ('You did not meet minimum requirements; your performance score will be zero.')


setup_working_directory()
check_software_engineering("threadpool.o", allowedsymbols)
number_of_existing_processes = count_number_of_processes()
if verbose:
    print ("There are %d process currently running for user %s" % (number_of_existing_processes, os.getlogin()))

if not ignore_if_not_idle:
    wait_for_load_to_go_down()
    
results = run_tests(tests)
if verbose:
    print_results(results)
if not silent:
    print_grade_table(results, tests)

write_results_to_json(results_file)
print ("Wrote full results to %s/%s" % (workdir, results_file))

