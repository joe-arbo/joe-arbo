#!/usr/bin/python2
#
# Script: stdriver.py
# Author: Patrick Boyd
#
# Purpose: Run point-valued python test scripts and generate a grade sheet
#
# Description:
# Runs a set of python tests as determined by the contents of the input file.
# The contents of the .txt file should appear as follows:
#   = test group 1
#   5 test_worth_5.py
#   10 test_worth 10.py
#   15 test_worth_15.py
#   = test group 2
#   20 test_worth 20.py
#   25 test_worth 25.py
#   ...
#
# These tests will then be spawned into their own subprocesses and ran,
# and their results will be recorded.  If the verbose option is enabled,
# error information will be displayed.
#

import getopt, os, sys, subprocess, re, json

# add directory in which script is located to python path
# resolve any symlinks
script_file = os.path.realpath(__file__)
script_dir = "/".join(script_file.split("/")[:-1])

verbose = False
output_spec_file = "./output_spec.py"
plugin_directory = ""
test_base = os.path.dirname(os.path.realpath(__file__))
milestone_test_file = "milestone.tst"
basic_test_file = "basic.tst"
advanced_test_file = "advanced.tst"
reduced_test_file = "reduced.tst"
testfilter = None
list_tests = False
print_json = False
sum_points = dict()

def usage():
    print """
Usage: python stdriver.py [options] [tests1.tst tests2.tst] ... 
    -b              Include basic tests
    -a              Include advanced tests
    -v              Verbose 
    -h              Show help 
    -o outputspec   Run using this output spec file
    -t testname     Run only tests whose names contains 'testname'
    -B directory    Set test base (default %s)
    -l              List available tests in test set
    """ % (test_base)

try:
    opts, args = getopt.getopt(sys.argv[1:], "hvmbjo:p:t:B:al", \
        ["help", "outputspec=", "test="])
except getopt.GetoptError, err:
    # print help information and exit:
    print str(err) # will print something like "option -a not recognized"
    usage()
    sys.exit(2)

for o, a in opts:
    if o == "-v":
        verbose = True
    elif o in ("-h", "--help"):
        usage()
        sys.exit()
    elif o in ("-t", "--test"):
        testfilter = a
    elif o in ("-l"):
        list_tests = True
    elif o in ("-B"):
        test_base = a
    elif o in ("-m"):
        args += ["%s/%s" % (test_base, milestone_test_file)]
    elif o in ("-b"):
        args += ["%s/%s" % (test_base, basic_test_file)]
    elif o in ("-a"):
        args += ["%s/%s" % (test_base, advanced_test_file)]
    elif o in ("-p"):
        plugin_directory = a
    elif o in ("-o"):
        output_spec_file = a
    elif o in ("-j"):
        print_json = True
    else:
        assert False, "unhandled option"

if plugin_directory != "" and not os.access(plugin_directory, os.R_OK):
    print "Directory ", plugin_directory, " is not readable"
    print "You must create this plugin directory if you wish to test plugins"
    sys.exit(1)
else:
    plugin_directory = " -p " + plugin_directory

if not os.access(output_spec_file, os.R_OK):
    print "File ", output_spec_file, " is not readable"
    print "You must create this file and adapt it to match the output of your shell"
    sys.exit(1)

full_testlist = []
if len(args) == 0:
    usage()
    sys.exit(1)

for testlist_filename in args:
    try:
        testlist_file = open(testlist_filename, 'r')
        test_dir = os.path.dirname(testlist_filename)
        if test_dir == "":
            test_dir = './'
        else:
            test_dir = test_dir + '/'
    except:
        print 'Error: Tests list file: ''%s'' could not be opened.'% testlist_filename
        sys.exit(-1)

    #File input, read in the test filenames
    #test information is placed into tuples that are placed onto a 2d list
    for line in testlist_file:

        grps = re.match("^= (.+)", line)
        if grps:
            testset = { 'name' : grps.group(1), 
                        'tests' : [ ], 
                        'dir' : test_dir }
            full_testlist.append(testset)
        else:
            grps = re.match("(\d+) (.+)", line)
            if grps:
                points, testname = int(grps.group(1)), grps.group(2)
                if not testfilter or testname.find(testfilter) != -1:
                    testset['tests'].append((points, testname))

    testlist_file.close()

# print full_testlist
if list_tests:
    for testset in full_testlist:
        print testset['name']
        for (points, testname) in testset['tests']:
            print ("{} pts {}".format(points, testname))

    sys.exit()

process_list = []

#Run through each test set in the list
for testset in full_testlist:
    print testset['name']
    print '-------------------------'
    
    #Run through each test in the set
    testresults = [ ]
    for (points, testname) in testset['tests']:
    
        print str(points) + '\t' + testname + ':',
        sys.stdout.flush()

        # augment PYTHONPATH so that our own version of pexpect and shellio
        # are picked up.
        augmented_env = dict(os.environ)
        augmented_env['PYTHONPATH'] = ":".join([
            script_dir + "/../pexpect-dpty/", 
            script_dir])
        
        # run test
        child_process = subprocess.Popen(["python2", testset['dir'] + testname, \
                             output_spec_file, plugin_directory],\
                             env=augmented_env,\
                             stderr=subprocess.PIPE, stdout=subprocess.PIPE)
        test_passed = child_process.wait() == 0

        # build result list
        testresults.append((points, testname, child_process, test_passed))
        
        #if: test passed
        if test_passed:
            print ' PASS'
        else:
            print ' FAIL'
    print ""
    testset['tests'] = testresults


    #Grade sheet printing.  It prints each individual test and the points awarded
    #from that test, the points awarded from each set of tests, and the total
    #points awarded over the entire test.

    #Run through each set of tests in the list
    testset_points = 0
    testset_points_earned = 0
    
    #Run through each test in the set
    for (points, testname, child_process, test_passed) in testset['tests']:
        testset_points += points
        points_earned = points
        if not test_passed:
            points_earned = 0

        print '\t%s\t%d/%d' % (testname, points_earned, points)
        testset_points_earned += points_earned
            
    print '-------------------------'
    
    print testset['name'] + '\t' + str(testset_points_earned) + '/' + \
                                                        str(testset_points)

    # Gather score info for json output if necessary
    if (print_json):
        sum_points[testset['name']] = dict()
        sum_points[testset['name']]['points'] = testset_points_earned
        sum_points[testset['name']]['max'] = testset_points

# Write scores.json to the current (src) directory
if (print_json):
    try:
        outfile = open('./scores.json', 'w')
        outfile.write(json.dumps(sum_points))
        outfile.close()
    except Exception as e:
        pass
#Verbose printing.  If the verbose option was enabled, print the error
#information from the tests that failed.
if verbose:
    print '\nError Output'
    print '-------------------------'
    for testset in full_testlist:
        for (points, testname, child_process, test_passed) in testset['tests']:
            if not test_passed:
                print testname + ':'
                (stdout, stderr) = child_process.communicate()
                print stdout, stderr

