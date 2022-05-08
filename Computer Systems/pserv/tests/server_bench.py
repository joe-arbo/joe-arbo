#!/usr/bin/env python3

#
# Benchmark an HTTP server with wrk
#
# @author gback; Spring 2016, Spring 2018, Summer 2020
#

import getopt, sys, os, subprocess, signal, re, json, resource, time, socket, atexit, traceback
from collections import namedtuple

from http.client import HTTPConnection, OK

runconfig = namedtuple(
    "runconfig",
    [
        "name",  # name of configuration
        "nthreads",  # number of threads
        "nconnections",  # number of conn per thread
        "timeout",  # number of conn per thread
        "window",  # number of conn per thread
        "overlap",  # number of conn per thread
        "description",  # description
        "path",  # target path to be retrieved
        "duration",
    ],
)  # duration (with unit as string, i.e. "1s")

VERSION = "1.1"
server_exe = "./server"
server_root = "_serverroot_"
wrk_exe = "/home/courses/cs3214/bin/wrk"
nthreads = 64

# tests will be run in this order
tests = [
    runconfig(
        name="login40",
        timeout="1s",
        nthreads=40,
        nconnections=40,
        duration="10s",
        path="/api/login",
        description="""
    Can your server handle 40 parallel connections request /api/login?
    """,
        window=500,
        overlap=250,
    ),
    runconfig(
        name="login500",
        timeout="5s",
        nthreads=nthreads,
        nconnections=500,
        duration="10s",
        path="/api/login",
        description="""
    Using 500 connections, each of which is repeatedly requesting /api/login (~2bytes in
    HTTP body).  We believe this should be enough to make the server CPU bound.
    """,
        window=1000,
        overlap=500,
    ),
    runconfig(
        name="login10k",
        timeout="5s",
        nthreads=nthreads,
        nconnections=10000,
        duration="30s",
        path="/api/login",
        description="""
    Handling 10k simultaneous connections has been a target of scalability since 1999:
    http://www.kegel.com/c10k.html
    Can your server handle it?
    """,
        window=1000,
        overlap=500,
    ),
    runconfig(
        name="wwwcsvt100",
        timeout="5s",
        nthreads=nthreads,
        nconnections=100,
        duration="20s",
        path="/www.cs.vt.edu-20200417.html",
        description="""
    The home page of the CS Department, as of 4/17/2020, is about 66KB large (not counting embedded objects).
    If 100 clients accessed it simultaneously, how much throughput could they expect?
    """,
        window=1000,
        overlap=500,
    ),
    runconfig(
        name="doom100",
        timeout="10s",
        nthreads=40,
        nconnections=40,
        duration="20s",
        path="/large",
        description="""
    According to https://mobiforge.com/research-analysis/the-web-is-doom the combined size of all
    objects that make an average web page was 2,250kBytes as of April 2016. If these were transferred
    all in a single objects, how much throughput would you get?
    This should max out the 10Gbps Ethernet links, even with only 40 connections.
    """,
        window=500,
        overlap=250,
    ),
]
testsbyname = dict((c.name, c) for c in tests)
teststorun = map(lambda t: t.name, tests)


def listtests():
    for test in tests:
        print(
            """
Test:           %s
Connections:    %d
Duration:       %s
Path:           %s
Description:    %s
"""
            % (test.name, test.nconnections, test.duration, test.path, test.description)
        )


script_dir = "/".join(os.path.realpath(__file__).split("/")[:-1])
if script_dir == "":
    script_dir = "."

script_dir = os.path.realpath(script_dir)


def usage():
    print(
        """
Usage: %s [-hv] [-l] [-s server] [-R serverroot] [-t test1,test2,...] [url]

   -h                     display this help
   -v                     run verbose
   -s                     path to server executable, default %s
   -R server_root         path to server root, default %s
   -t test                run just the tests specified
   -l                     list available tests with their descriptions
   -i                     activate ink tracing tool
   url                    URL where your server can be reached, i.e.
                            http://hickory.rlogin:12306/

This script must be started on two different rlogin nodes.
On the first node, run it without a URL to start the server.

Then run it on a second node with the URL printed out by the
first run.

    """
        % (sys.argv[0], server_exe, server_root)
    )


try:
    opts, args = getopt.getopt(sys.argv[1:], "ihvs:R:t:l", ["help", "verbose"])
except getopt.GetoptError as err:
    print(str(err))
    usage()
    sys.exit(2)

verbose = False
hostname = socket.gethostname()
useInk = False

for opt, arg in opts:
    if opt == "-h":
        usage()
        sys.exit(0)
    if opt == "-v":
        verbose = True
    elif opt == "-i":
        useInk = True
    elif opt == "-s":
        server_exe = arg
    elif opt == "-R":
        server_root = arg
    elif opt == "-l":
        listtests()
        sys.exit(0)
    elif opt == "-t":
        teststorun = arg.split(",")
    else:
        assert False, "unhandled option"


def raise_fd_limit():
    print("I will now try to raise the file descriptor limit")
    soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
    resource.setrlimit(resource.RLIMIT_NOFILE, (hard, hard))
    soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
    print("Your server process can open %d file descriptors simultaneously." % soft)


def raise_thread_limit():
    print("I will now try to raise the max number of threads you can spawn")
    soft, hard = resource.getrlimit(resource.RLIMIT_NPROC)
    resource.setrlimit(resource.RLIMIT_NPROC, (hard, hard))
    soft, hard = resource.getrlimit(resource.RLIMIT_NPROC)
    print("Your server process can spawn %d threads simultaneously." % soft)


#
# Start the server.
#
def start_server(root_dir):
    print("I will now prepare your server for benchmarking.")
    if not os.access(server_exe, os.X_OK):
        print("Did not find server executable: %s" % (server_exe))
        sys.exit(-1)

    # prepare files to be served
    print("I will use the directory %s to store 2 files" % (root_dir))
    if not os.access(root_dir, os.W_OK):
        os.mkdir(root_dir)

    def make_synthetic_content(sz):
        return "0123456789ABCDEF" * int(sz / 16)

    def write_file(name, content):
        with open("%s/%s" % (root_dir, name), "wb") as sfile:
            sfile.write(content.encode("utf-8"))
            sfile.close()

    sfilecontent = make_synthetic_content(1024)
    write_file("small", sfilecontent)
    lfilecontent = make_synthetic_content(2250 * 1024)
    write_file("large", lfilecontent)
    wwwcscont = open("%s/res/www.cs.vt.edu-20200417.html" % script_dir).read()
    write_file("www.cs.vt.edu-20200417.html", wwwcscont)

    port = (os.getpid() % 10000) + 20000

    cmd = [server_exe, "-p", str(port), "-R", root_dir, "-s"]

    raise_fd_limit()
    raise_thread_limit()

    server = subprocess.Popen(cmd, stdout=open(os.devnull, "w"), stderr=sys.stderr)

    def clean_up_testing():
        try:
            os.kill(server.pid, signal.SIGKILL)
        except:
            pass

    atexit.register(clean_up_testing)

    print("I will now test that your server works.")

    def test_server():
        http_conn = HTTPConnection(hostname, port)
        http_conn.connect()
        for url, expected in zip(
            ["/small", "/large", "/www.cs.vt.edu-20200417.html", "/api/login"],
            [sfilecontent, lfilecontent, wwwcscont, "{}"],
        ):
            http_conn.request("GET", url)
            server_response = http_conn.getresponse()
            sfile = server_response.read().decode("utf-8")
            if server_response.status != OK:
                print(
                    "Server returned %s for %s, expected %d."
                    % (server_response.status, url, OK)
                )
                sys.exit(-1)

            if (
                isinstance(expected, int)
                and len(sfile) >= expected
                or isinstance(expected, (str, bytes))
                and sfile == expected
            ):
                print("Retrieved %s ok." % (url))
            else:
                print("Did not find expected content at %s." % (url))
                sys.exit(-1)

        http_conn.close()

    for tries in range(10):
        try:
            time.sleep(1)
            test_server()
            break
        except Exception as e:
            print(f'starting server failed: {e}')
            pass

    if tries == 9:
        print("Your server did not start, giving up after 10 tries")
        sys.exit(0)

    this_script = os.path.realpath(sys.argv[0])
    print(
        f"""
Congratulations, you are now ready to run the benchmark!
Now, find another unloaded rlogin machine and run:

{this_script} http://{hostname}:{port}/

To use the ink tool, instead run 

{this_script} -i http://{hostname}:{port}/

When you are done, don't forget to hit ^C here.

Your server's stdout is going to /dev/null.
Your server's stderr is going to the driver's stderr.
"""
    )
    sys.stdout.flush()
    server.wait()


def start_wrk(url, test):

    cmd = [ wrk_exe ]
    cmd += [] if useInk else ['--no-trace']
    cmd += [
        "-c", str(test.nconnections),
        "-t", str(test.nthreads),
        "-d", test.duration,
        "-r", test.name + ".tar",
        "-x", test.timeout,
        "-w", str(test.window),
        "-o", str(test.overlap),
        "-s", script_dir + "/cs3214bench.lua",
        url + test.path,
    ]
    if verbose:
        print("I will now run", " ".join(cmd))

    resfile = "ssresults.json"
    luajson = "%s/JSON.lua" % script_dir
    assert os.access(luajson, os.R_OK)
    server = subprocess.Popen(
        cmd,
        stdout=sys.stdout,
        stderr=sys.stderr,
        env=dict(os.environ, JSON_OUTPUT_FILE=resfile, JSON_LUA=luajson),
    )
    server.wait()
    with open(resfile) as jfile:
        r = json.load(jfile)
        os.unlink(resfile)
        return r


if len(args) == 0:
    start_server(server_root)
else:
    url = args[0]
    # strip ending / since the path args contain them
    while url.endswith("/"):
        url = url[:-1]

    if hostname in url:
        print("Please do not start the client on the same machine as the server.")
        sys.exit(-1)

    raise_fd_limit()
    raise_thread_limit()
    results = dict(version=VERSION)
    ran = []
    for testname in teststorun:
        ran.append(testname)
        if testname not in testsbyname:
            print("Test: %s not found, skipping" % testname)
            continue

        test = testsbyname[testname]
        print("Now running test: %s\n" % (testname))
        try:
            results[testname] = start_wrk(url, test)
        except Exception as e:
            # print the backtrace
            traceback.print_exc(file=sys.stderr)
            print("An exception occurred %s, skipping this test" % (str(e)))

    ofilename = "pserv.results.%d.json" % (os.getpid())
    print("Writing results to %s" % ofilename)
    with open(ofilename, "w") as ofile:
        json.dump(results, ofile)

    print(
        """
    Submit your results to the scoreboard with ~cs3214/bin/sspostresults.py %s
        """
        % ofilename
    )

    with open(ofilename, "r") as f:
        data = json.load(f)

    score = 0
    # the following rubric encode the performance expectations for this semester
    # there are 20 pts currently, 4 pts per benchmark.
    #
    # If errors (p) is set, deduct p pts if there are one or more errors
    #
    # If served (a, b) is set, deduct b pts unless a fraction of a clients was served
    #
    rubric = {
        # 200 or more yields 4 pts, 100 or more yields 2 points
        "login40": {"rps": [(320, 4), (160, 2)], "errors": 1},
        "login500": {"rps": [(800, 4), (500, 2)], "errors": 1},
        "login10k": {"rps": [(650, 4), (450, 2)], "served": (0.80, 2)},
        # these max out the 10GBps link, so these are MByte/s
        "wwwcsvt100": {"mbps": [(900, 4), (800, 2)]},
        "doom100": {"mbps": [(900, 4), (800, 2)]},
    }
    extra = ""
    for test in ran:
        points = rubric[test]
        category = 0
        if "rps" in points:
            rps = 1e3 * (
                data[test]["summary"]["requests"] / data[test]["summary"]["duration"]
            )
            for requiredmin, value in points["rps"]:
                if rps > requiredmin:
                    category += value
                    break
            if test == "login10k" and rps > 1000:
                extra = "+10 points extra credit for supporting more than 1 million rqs for login10k! If your error count isn't > 5,000..."
        if "mbps" in points:
            mbps = (
                1e6
                * data[test]["summary"]["bytes"]
                / data[test]["summary"]["duration"]
                / 1024
                / 1024
            )
            for requiredmin, value in points["mbps"]:
                if mbps > requiredmin:
                    category += value
                    break
        if "served" in points:
            (threshold, deduction) = points["served"]
            percent = (
                data[test]["summary"]["served"] / data[test]["summary"]["connections"]
            )
            if percent < threshold:
                category = max(category - deduction, 0)
        if "errors" in rubric[test]:
            errors = sum(data[test]["summary"]["errors"].values())
            if errors > 0:
                category = max(category - rubric[test]["errors"], 0)
        print("%s: %d/4" % (test, category))
        score += category

    print("Your server got a performance score of %d/20" % score)
    if extra != "":
        print(extra)

    print(
        """
    Submit your individual <test_name>.tar reports with ~cs3214/bin/p4api.sh\nThis will return a link to visualize your server's performance.
        """
    )
