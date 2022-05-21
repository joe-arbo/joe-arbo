from collections import namedtuple

# Test definitions

thread_count = [1, 2, 4, 8, 16]
__threadpool_test = namedtuple('threadpool_test',
                             ['name',           # name of the test (quicksort, fib, etc)
                              'command',        # command line to execute
                              'description',    # description of test
                              'is_required',     # is this test part of the minimum requirements?
                              'limit_threads', # limit number of threads
                              'runs'])

def threadpool_test(name, command, description, runs, is_required=False, limit_threads=True):
    return __threadpool_test(name, command, description, is_required, limit_threads, runs)

__test_run = namedtuple('test_run', [
    'name', 'args', 'thread_count', 'input_file', 'is_benchmarked', 'timeout'
])

def test_run(name, args, thread_count=thread_count, input_file=None, is_benchmarked=False, timeout=15):
    return __test_run(name, args, thread_count, input_file, is_benchmarked, timeout)

def load_test_module(name):
    return __import__('fjtests.' + name, fromlist=[name]).tests
