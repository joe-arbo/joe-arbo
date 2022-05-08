#!/usr/bin/env python3
#
# The purpose of this class is to drive unit tests against a server that
# handles requests for system statistics.  Unit tests will cover a number
# of areas, described as the following suites of tests:
#
#   1.  Correctness for good requests
#   2.  Correctness for expectable bad requests
#   3.  Malicious request handling
#
#

import atexit, base64, errno, getopt, json, multiprocessing, os
import random, requests, signal, socket, struct, string, subprocess
import sys, time, traceback, unittest, re

from datetime import datetime
from fractions import Fraction as F
from multiprocessing.dummy import Pool as ThreadPool
from socket import error as SocketError

from http.client import OK, NOT_FOUND, FORBIDDEN, BAD_REQUEST, METHOD_NOT_ALLOWED, NOT_IMPLEMENTED, HTTPConnection
random.seed(42)

script_dir = "/".join(os.path.realpath(__file__).split("/")[:-1])
if script_dir == "":
    script_dir = "."
script_dir = os.path.realpath(script_dir)

def encode(s):
    return s.encode('utf-8')

def get_socket_connection(hostname, port):
    """
    Connect to a server at hostname on the supplied port, and return the socket
    connection to the server.
    """
    for res in socket.getaddrinfo(hostname, port, socket.AF_UNSPEC, socket.SOCK_STREAM):
        family, sockettype, protocol, canonname, socketaddress = res
        try:
            sock = socket.socket(family, sockettype, protocol)
            sock.settimeout(10)
            # avoid TCP listen overflows when making back-to-back requests 
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 1))

        except socket.error as msg:
            sock = None
            continue

        try:
            sock.connect(socketaddress)
        except socket.error as msg:
            sock.close()
            sock = None
            continue

        break

    if sock is None:
        raise ValueError('The script was unable to open a socket to the server')
    else:
        return sock


def run_connection_check_loadavg(http_conn, hostname):
    """
    Run a check of the connection for validity, using a well-formed
    request for /loadavg and checking it after receiving it.
    """

    # GET request for the object /loadavg
    http_conn.request("GET", "/loadavg", headers={"Host": hostname})

    # Get the server's response
    server_response = http_conn.getresponse()

    # Check the response status code
    assert server_response.status == OK, "Server failed to respond"

    # Check the data included in the server's response
    assert check_loadavg_response(server_response.read().decode('utf-8')), \
        "loadavg check failed"


def run_connection_check_empty_login(http_conn, hostname):
    """
    Run a check of the connection for validity, using a well-formed
    request for /api/login and checking it after receiving it.
    """

    # GET request for the object /api/login
    http_conn.request("GET", "/api/login", headers={"Host": hostname})

    # Get the server's response
    server_response = http_conn.getresponse()

    # Check the response status code
    assert server_response.status == OK, "Server failed to respond"

    # Check the data included in the server's response
    assert check_empty_login_respnse(server_response.read().decode('utf-8')), \
        "empty login check failed"

def run_404_check(http_conn, obj, hostname):
    """
    Checks that the server properly generates a 404 status message when
    requesting a non-existent URL object.
    """

    # GET request for the object /loadavg
    http_conn.request("GET", obj, headers={"Host": hostname})

    # Get the server's response
    server_response = http_conn.getresponse()

    # Check the response status code
    assert server_response.status == NOT_FOUND, \
        "Server failed to respond with a 404 status for obj=" + obj + ", gave response: " + str(server_response.status)
    server_response.read()


def run_query_check(http_conn, request, req_object, callback, hostname):
    """
    Checks that the server properly processes the query string passed to it.
    """

    http_conn.request("GET", request, headers={"Host": hostname})
    server_response = http_conn.getresponse()
    assert server_response.status == OK, "Server failed to respond"

    if callback is None:
        if req_object == "loadavg":
            assert check_loadavg_response(server_response.read().decode('utf-8')), \
                "loadavg check failed"
        else:
            assert check_meminfo_response(server_response.read().decode('utf-8')), \
                "meminfo check failed"
    else:
        assert check_callback_response(server_response.read().decode('utf-8'),
                                       callback, req_object), "callback check failed"


def run_method_check(http_conn, method, hostname):
    """
    Check that the unsupported method supplied has either a NOT IMPLEMENTED
    or METHOD NOT ALLOWED response from the server.
    """

    http_conn.request(method, "/api/login", headers={"Host": hostname})
    server_response = http_conn.getresponse()
    assert (server_response.status == METHOD_NOT_ALLOWED or
            server_response.status == NOT_IMPLEMENTED), \
        "Server failed to respond with the METHOD NOT ALLOWED or \
        NOT IMPLEMENTED status for method: " + method + " response was: " \
        + str(server_response.status)
    server_response.read()


def print_response(response):
    """Print the response line by line as returned by the server.  the response
    variable is simply the server_response.read(), and this function prints out
    each line of the output.  Most helpful for printing an actual web page. """

    lines = response.split("\n")
    for line in lines:
        print(line.strip())


def check_loadavg_response(response):
    """Check that the response to a loadavg request generated the correctly
    formatted output.  Returns true if it executes properly, throws an
    AssertionError if it does not execute properly or another error if json
    is unable to decode the response."""

    try:
        data = json.loads(response.strip())
    except ValueError as msg:
        raise AssertionError("Invalid JSON object.  Received: " + response)

    assert len(data) == 3, "Improper number of data items returned"

    assert 'total_threads' in data, "total_threads element missing"
    assert 'loadavg' in data, "loadavg element missing"
    assert 'running_threads' in data, "running_threads element missing"

    assert len(data['loadavg']) == 3, 'Improper number of data items in \
        loadavg'

    return True


def check_meminfo_response(response):
    """Check that the response to a meminfo request generated the correctly
    formatted output.  Returns true if it executes properly, throws an
    AssertionError if it does not execute properly or another error if json
    is unable to decode the response."""

    try:
        data = json.loads(response.strip())
    except ValueError as msg:
        raise AssertionError("Invalid JSON object.  Received: " + response)

    for line in open("/proc/meminfo"):
        entry = re.split(":?\s+", line)
        assert entry[0] in data, entry[0] + " key is missing"

        try:
            int(data[entry[0]])
        except (TypeError, ValueError):
            raise AssertionError("a non-integer was passed to meminfo")

    return True


def check_callback_response(response, callback, req_obj):
    """Check that the response to a req_obj request with callback function
    callback generated the correctly formatted output.  Returns true if it 
    executes properly, throws an AssertionError if it does not execute properly
    or another error if json is unable to decode the response."""
    callback.replace(' ', '')
    response.replace(' ', '')
    assert response[0:len(callback) + 1] == callback + "(", 'callback incorrect, was: ' + response[0:len(
        callback) + 1] + ' , expected: ' + callback + '('
    assert response[len(response) - 1] == ")", 'missing close parenthesis'

    if req_obj == "meminfo":
        check_meminfo_response(response[len(callback) + 1:len(response) - 1])
    elif req_obj == "loadavg":
        check_loadavg_response(response[len(callback) + 1:len(response) - 1])
    else:
        return False

    return True


def check_meminfo_change(response1, response2, key, delta, safety_margin=0):
    """Check that a specific value in meminfo has changed by at least some amount.
    Used by allocanon and freeanon tests. Returns true if it executes properly,
    throws an AssertionError if it does not execute properly or another error
    if json is unable to decode the response."""

    check_meminfo_response(response1)
    check_meminfo_response(response2)

    data1 = json.loads(response1.strip())
    data2 = json.loads(response2.strip())

    if delta >= 0:
        return float(data2[key]) - float(data1[key]) > delta * (1 - safety_margin)
    else:
        return float(data2[key]) - float(data1[key]) < delta * (1 - safety_margin)


def check_empty_login_respnse(response):
    return response.strip() == "{}"

ld_preload = f'{script_dir}/getaddrinfo.so.1.0.1'
if not os.path.exists(ld_preload):
    print (f"Couldn't find ${ld_preload}, please run (cd {script_dir}; ./build.sh)")
    sys.exit(1)

def usage():
    print("""
    Usage: python3 server_unit_test_pserv.py -s server [-h, -t testname, -o outfile]
        -h              Show help
        -s server       File path to the server executable
        -t testname     Run a test by itself, its name given as testname
        -l              List available tests
        -6 host         Hostname of IPv6 localhost (default: localhost6)
        -o outputfile   Send output from the server to an output file
          """)


def handle_exception(type, exc, tb):
    """Install a default exception handler.
    If there is an exception thrown at any time in the script,
    report that the test failed, close the server and exit.
    """
    print("\n>>> FAIL: ", type, "'", exc, "'\n")
    print(type.__doc__ + "\n")
    traceback.print_tb(tb)


def decode_base64(data):
    """Decode base64, padding being optional.

    :param data: Base64 data as a string
    :returns: The decoded byte string.

    Adapted from https://stackoverflow.com/a/9807138
    """
    data = data.encode()
    missing_padding = len(data) % 4
    if missing_padding != 0:
        data += b'='* (4 - missing_padding)

    return base64.b64decode(data)


# Install the default exception handler
sys.excepthook = handle_exception


##############################################################################
## Class: Doc_Print_Test_Case
## Extending the unittest.TestCase class for a better print of the __doc__
## type of each test method.
##
#
# TBD: investigate if this method was only used in Python 2.4 and isn't
# already part of TestCase in unittest in Python 2.6
#
##############################################################################

class Doc_Print_Test_Case(unittest.TestCase):
    def __init__(self, methodName='runTest'):
        """
        Overriding the super-class __init__ because it uses an internal
        attribute for the test method doc that is not inherited.
        """
        unittest.TestCase.__init__(self, methodName)

    def shortDescription(self):
        """
        Returns the __doc__ of the test method, instead of unittest.TestCase's
        standard action of returning the first line of the test method.  This
        will allow for more verbose testing with each method.
        """
        return self._testMethodDoc

##############################################################################
## Class: Single_Conn_Protocol_Case
## test cases that ensure HTTP/1.0 connections close automatically,
## and HTTP/1.1 connections have persistent connections.
##############################################################################

class Single_Conn_Protocol_Case(Doc_Print_Test_Case):
    """
    Test case for a single connection, checking various points of protocol
    usage that ensures the servers to be HTTP 1.0 and 1.1 compliant.
    Each case should be handled without the server crashing.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Single_Conn_Protocol_Case, self).__init__(testname)
        self.hostname = hostname
        self.port = port

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: None.  An error here \n\
                   means the server crashed after servicing the request from \n\
                   the previous test.
        """
        if server.poll() is not None:
            # self.fail("The server has crashed.  Please investigate.")
            print("The server has crashed.  Please investigate.")

    def test_http_1_0_compliance(self):
        """  Test Name: test_http_1_0_compliance\n\
        Number Connections: 1 \n\
        Procedure: Writes "GET /api/login HTTP/1.0\\r\\n" to the server, then \n\
                   checks nothing has been returned, and finishes with the \n\
                   extra "\\r\\n" and checking the data sent back from the \n\
                   server.
        """
        # Make HTTP connection for the server
        sock = get_socket_connection(self.hostname, self.port)

        sock.send(encode("GET /api/login HTTP/1.0\r\n"))
        sock.send(encode("Host: " + self.hostname + "\r\n"))
        sock.settimeout(1)
        time.sleep(.1)
        try:
            if sock.recv(4096, socket.MSG_PEEK).decode('utf8') != '':
                self.fail("The http response was returned too early, before" + \
                          " the extra \r\n line.")

        except socket.timeout:
            pass

        sock.send(encode("\r\n"))
        # If there is a HTTP response, it should be a valid /loadavg
        # response.
        data = ""

        time.sleep(0.1)
        try:
            while sock.recv(4096, socket.MSG_PEEK).decode('utf8') != '':
                msg_buffer = sock.recv(4096).decode('utf8')
                data = data + msg_buffer

        # Connections close after responses for HTTP/1.0 , therefore a timeout
        # should not occur.
        except socket.timeout:
            self.fail("The server did not respond and close the connection in sufficient time.")

        data = data.split("\r\n\r\n")
        assert len(data) == 2, \
            "The response could not be parsed, check your use of \\r\\n"

        assert check_empty_login_respnse(data[1]), \
            "The /login object was not properly returned."

        sock.close()

    def test_http_1_1_compliance(self):
        """  Test Name: test_http_1_1_compliance\n\
        Number Connections: 1 \n\
        Procedure: Ensure a persistent connection by sending seven consecutive\n\
                   requests to the server on one connection.
        """
        # Make HTTP connection for the server
        self.http_connection = HTTPConnection(self.hostname, self.port)
        self.http_connection.auto_open = 0

        # Connect to the server
        self.http_connection.connect()

        for x in range(0, 7):
            # GET request for the object /login
            self.http_connection.request("GET", "/api/login")

            # Get the server's response
            server_response = self.http_connection.getresponse()

            # Check that the server did not close the connection
            # this will be True if the server responds with a HTTP/1.1
            # independent of whether the connection has been closed or not.
            self.assertEqual(server_response._check_close(), False, \
                             "Server closed the connection")

            # Check the response status code
            self.assertEqual(server_response.status, OK, "Server failed to respond")

            # Check the data included in the server's response
            self.assertTrue(check_empty_login_respnse(server_response.read().decode('utf8')), \
                            "empty login response check failed")

        self.http_connection.close()


##############################################################################
## Class: Single_Conn_Malicious_Case
## Test cases that are attempting to break down the server
##############################################################################

class Single_Conn_Malicious_Case(Doc_Print_Test_Case):
    """
    Test case for a single connection, using particularly malicious requests
    that are designed to seek out leaks and points that lack robustness.
    Each case should be handled without the server crashing.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Single_Conn_Malicious_Case, self).__init__(testname)
        self.hostname = hostname
        self.port = port
        self.private_file = 'private/secure.html'
        self.username = 'user0'
        self.password = 'thepassword'

    def setUp(self):
        """  Test Name: None -- setUp function\n\
        Number Connections: N/A \n\
        Procedure: Create requests session.
        """
        # Create a requests session
        self.session = requests.Session()

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: An error here \
                   means the server crashed after servicing the request from \
                   the previous test.
        """

        if server.poll() is not None:
            # self.fail("The server has crashed.  Please investigate.")
            print("The server has crashed.  Please investigate.")
        # Close the requests session
        self.session.close()



    def test_multi_connection_disconnect(self):
        """  Test Name: test_multi_connection_disconnect\n\
        Number Connections: 5 \n\
        Procedure: tries to do a login GET request but fails and closes connection at some point during
        the request response sequence.
        """
        request_string = f'GET /api/login HTTP/1.1\r\nHost: {self.hostname}\r\n\r\n'.encode()
        nconnections = len(request_string)
        connection_list = [ (get_socket_connection(self.hostname, self.port), i) for i in range(nconnections) ]
        random.shuffle(connection_list)

        # write one character to each until they are at the failure point
        for idx in range(len(request_string)):
            for sock, stop_idx in connection_list:
                if idx < stop_idx:
                    sock.send(request_string[idx:idx+1])

        for sock, _ in connection_list:
            sock.close()

        # since we are using raw sockets here, we can't and don't test the result.
        # it suffices if the server doesn't crash

    def test_file_descriptor_leak(self):
        """  Test Name: test_file_descriptor_leak\n\
        Number Connections: 4000, but only one is connected at a time \n\
        Procedure: 4000 connections are processed as follows: \n\
            1.  Make the connection\n\
            2.  Test a /api/login request\n\
            3.  Close the connection\n\
        IMPORTANT NOTE: May also thread/fork-bomb your server!
        """
        start = time.time()
        for x in range(4000):
            http_connection = HTTPConnection(hostname, port)
            # avoid TCP listen overflows
            http_connection.connect()
            http_connection.sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 1))

            # GET request for the object /login
            http_connection.request("GET", "/api/login")

            # Get the server's response
            server_response = http_connection.getresponse()

            # Check the response status code
            assert server_response.status == OK, "Server failed to respond"

            # Check the data included in the server's response
            assert check_empty_login_respnse(server_response.read().decode('utf8')), \
                "api/login check failed"
            http_connection.close()
            if time.time() - start > 60:
                raise AssertionError("Timeout - took more than 60 seconds")

    def test_file_descriptor_early_disco_leak_1(self):
        """  Test Name: test_file_descriptor_early_disco_leak_1\n\
        Number Connections: 4000, but only one is connected at a time \n\
        Procedure: 4000 connections are processed as follows: \n\
            1.  Make the connection\n\
            2.  Send to the server: GET /api/login HTTP/1.1\\r\\n\n\
                  NOTE: Only ONE \\r\\n is sent!\n\
            3.  Close the connection\n\
        IMPORTANT NOTE: May also thread/fork-bomb your server!
        """
        # Test note: the failure will be induced if get_socket_connection
        # is unable to create a new connection, and an assertion error is thrown
        start = time.time()
        for x in range(4000):
            socket = get_socket_connection(self.hostname, self.port)

            # Write to the server
            socket.send(encode("GET /api/login HTTP/1.1\r\n"))
            socket.send(encode("Host: " + self.hostname + "\r\n"))
            # Close the socket
            socket.close()
            if time.time() - start > 60:
                raise AssertionError("Timeout - took more than 60 seconds")

    def test_file_descriptor_early_disco_leak_2(self):
        """  Test Name: test_file_descriptor_early_disco_leak_2\n\
        Number Connections: 2000, but only one is connected at a time \n\
        Procedure: 2000 connections are processed as follows: \n\
            1.  Make the connection\n\
            2.  Send to the server: GET /api/login HTTP/1.1\n\
                  NOTE: NO \\r\\n's are sent!\n\
            3.  Close the connection\n\
        IMPORTANT NOTE: May also thread/fork-bomb your server!
        """

        # Test note: the failure will be induced if get_socket_connection
        # is unable to create a new connection, and an assertion error is thrown
        start = time.time()
        for x in range(4000):
            socket = get_socket_connection(self.hostname, self.port)

            # Write to the server
            socket.send(encode("GET /api/login HTTP/1.1"))

            # Close the socket
            socket.close()
            if time.time() - start > 60:
                raise AssertionError("Timeout - took more than 60 seconds")

    def test_80_kb_URI(self):
        """  Test Name: test_80_kb_URI\n\
        Number Connections: 1\n\
        Procedure: Send a GET request for a URI object that is 80kb long.\n\
                   Then check that another connection and request can still\n\
                   be made.  Also, ensure that an appropriate response is\n\
                   sent to the 80kb request.\n\
        """

        sock = get_socket_connection(self.hostname, self.port)

        sock.send(encode("GET "))

        data = ''
        try:
            for x in range(1, 10240):
                sock.send(encode("/api/login"))

            sock.send(encode(" HTTP/1.1\r\n"))
            sock.send(encode("Host: " + self.hostname + "\r\n\r\n"))

            # If there is a HTTP response, it should NOT be a valid /api/login
            # response.  All other responses are fine, including closing the
            # connection, so long as the server continues serving other connections
            sock.settimeout(1)
            data = ""

            time.sleep(0.1)
            while sock.recv(4096, socket.MSG_PEEK).decode('utf8') != '':
                msg_buffer = sock.recv(4096).decode('utf8')
                data = data + msg_buffer

                # Socket timeouts are not expected for HTTP/1.0 , therefore an open
                # connection is bad.
        except socket.timeout:
            pass
        except SocketError as e:
            if e.errno != errno.ECONNRESET:
                raise

        data = data.split("\r\n\r\n")

        try:
            if len(data) >= 2 and check_empty_login_respnse(data[1]):
                self.fail("A valid /api/login object was returned for an invalid request.")

        # If an error is generated, it comes from trying to an interpret a JSON
        # object that doesn't exist.
        except (AssertionError, ValueError):
            pass

        sock.close()

        # Make HTTP connection for the server
        self.http_connection = HTTPConnection(self.hostname, self.port)

        # Connect to the server
        self.http_connection.auto_open = 0
        self.http_connection.connect()

        # GET request for the object /api/login
        self.http_connection.request("GET", "/api/login")

        # Get the server's response
        server_response = self.http_connection.getresponse()

        # Check the response status code
        self.assertEqual(server_response.status, OK, "Server failed to respond")

        # Check the data included in the server's response
        self.assertTrue(check_empty_login_respnse(server_response.read().decode('utf8')), \
                        "api/login check failed")

        self.http_connection.close()

    def test_byte_wise_request(self):
        """  Test Name: test_byte_wise_request\n\
        Number Connections: 1\n\
        Procedure: Send a request for GET /api/login HTTP/1.1 byte by byte.\n\
        """

        # Make the low-level connection
        sock = get_socket_connection(self.hostname, self.port)

        for x in "GET /api/login HTTP/1.0\r\nHost: " + self.hostname + "\r\n":
            sock.send(encode(x))
            time.sleep(0.1)

        sock.settimeout(1)
        msg_buffer = ''
        try:
            if sock.recv(4096, socket.MSG_PEEK).decode('utf8') != '':
                self.fail("Data was returned before the extra \r\n")

        # We want nothing back until after we've sent the last \r\n
        except socket.timeout:
            pass

        if msg_buffer != '':
            self.fail("The server responded before the full request was sent.")

        sock.send(encode("\r"))
        sock.send(encode("\n"))

        time.sleep(0.1)
        # Collect the response
        try:
            while sock.recv(4096, socket.MSG_PEEK).decode('utf8') != '':
                data = sock.recv(4096).decode('utf8')
                msg_buffer = msg_buffer + data

                # Check the response
                data = data.split("\r\n\r\n")
        except socket.timeout:
            self.fail("The socket timed out on responding to the message.")
            return

        if len(data) == 2 and check_empty_login_respnse(data[1]):
            pass
        elif len(data) != 2:
            self.fail("The server did not return the proper api/login data")
        else:
            self.fail("A proper login object was not returned.")

        sock.close()
    
    def test_auth_flipped_token(self):
        """ Test Name: test_auth_flipped_token
        Number Connections: N/A
        Procedure: Checks if JSON parsing appropriately covers the case
                   when the correct key/value pair is present, but the
                   order in which it appears is flipped.
                   For example, typically one might expect:
                     '{"username": "<username>", "password": "<password>"}'
                   This tests for parsing of:
                     '{"password": "<password>", "username": "<username>"}'
        """
        # Login using the default credentials, with the password appearing
        # first in the JWT, followed by the username
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'password': self.password, 'username': self.username},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is authenticated
        self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server did not respond with private file despite being authenticated.")

    def test_auth_wrong_cookie(self):
        """ Test Name: test_auth_wrong_cookie
        Number Connections: N/A
        Procedure: Sends multiple requests with different cookies:
                     0. (First, a POST to /api/login to retrieve a valid cookie)
                     1. A cookie with the wrong name and correct JWT value
                     2. A cookie with the wrong name and wrong JWT value
                     3. Two cookies, both with wrong names and values
                     4. One wrong cookie, and one correct cookie.
                   Requests 1-3 should NOT be allowed to access a private file.
                   Request 4 SHOULD be allowed to access a private file.
                   (Note: a "wrong" name/value means a name/value that doesn't
                   equal the cookie returned by a successful POST to /api/login.)
        """
        # Login using the default credentials
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'username': self.username, 'password': self.password},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is authenticated
        self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # find the cookie returned by the server (its name and value)
        cookie_dict = self.session.cookies.get_dict()        # convert cookies to dict
        self.assertEqual(len(cookie_dict), 1, "Server did not return an authentication token.")
        cookie_key = list(cookie_dict.keys())[0]             # grab cookie's name
        cookie_val = self.session.cookies.get(cookie_key)    # grab cookie value
        # create a few "bad cookie" key-value pairs
        bad_cookie1_key = "a_bad_cookie1"
        bad_cookie1_val = "chocolate_chip"
        bad_cookie2_key = "a_bad_cookie2"
        bad_cookie2_val = "oatmeal_raisin"

        # ---------- test 1: same cookie value, different name ---------- #
        # append a string to the cookie name, making it the wrong cookie.
        # Then, clear the session cookies and add the wrong cookie
        self.session.cookies.clear()                         # wipe cookies
        self.session.cookies.set(bad_cookie1_key, cookie_val) # add wrong-named cookie
        
        # Use the INVALID cookie to try to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with private file despite not being authenticated.")
        
        # ----------- test 2: different cookie value AND name ----------- #
        # append a string to the cookie value. Then, update the bad cookie's value
        self.session.cookies.set(bad_cookie1_key, bad_cookie1_val)
        
        # Use the INVALID cookie to try to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with private file despite not being authenticated.")
        
        # ------------- test 3: multiple incorrect cookies -------------- #
        # set another cookie, so we end up sending TWO invalid cookies
        self.session.cookies.set(bad_cookie2_key, bad_cookie2_val)
        
        # Use the INVALID cookies to try to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with private file despite not being authenticated.")
        
        # --------- test 4: one bad cookie, one correct cookie ---------- #
        # clear the session cookies and add a bad cookie followed by a good cookie
        self.session.cookies.clear()
        self.session.cookies.set(bad_cookie1_key, bad_cookie1_val)
        self.session.cookies.set(cookie_key, cookie_val)
        
        # this time, even though we do have a wrong cookie, we also have the
        # correct cookie. So, the student's code *should* see this correct
        # cookie and allow us to get access to the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with private file despite being authenticated.")


##############################################################################
## Class: Single_Conn_Bad_Case
## Test cases that aim for various errors in well-formed queries.
##############################################################################

class Single_Conn_Bad_Case(Doc_Print_Test_Case):
    """
    Test case for a single connection, using bad requests that are
    well formed.  The tests are aptly named for describing their effects.
    Each case should be handled gracefully and without the server crashing.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Single_Conn_Bad_Case, self).__init__(testname)
        self.hostname = hostname
        self.port = port

        # Prepare the a_string for query checks
        self.a_string = "aaaaaaaaaaaaaaaa"
        for x in range(0, 6):
            self.a_string = self.a_string + self.a_string

        N = 10
        self.username = 'user0'
        self.invalid_username = ''.join(random.choice(string.ascii_lowercase + string.ascii_uppercase) for _ in range(N))
        self.password = 'thepassword'
        self.invalid_password = ''.join(random.choice(string.ascii_lowercase + string.ascii_uppercase) for _ in range(N))

    def setUp(self):
        """  Test Name: None -- setUp function\n\
        Number Connections: N/A \n\
        Procedure: Opens the HTTP connection to the server.  An error here \
                   means the script was unable to create a connection to the \
                   server.
        """
        # Make HTTP connection for the server
        self.http_connection = HTTPConnection(self.hostname, self.port)

        # Connect to the server
        self.http_connection.auto_open = 0
        self.http_connection.connect()

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: Closes the HTTP connection to the server.  An error here \
                   means the server crashed after servicing the request from \
                   the previous test.
        """
        # Close the HTTP connection
        self.http_connection.close()
        if server.poll() is not None:
            # self.fail("The server has crashed.  Please investigate.")
            print("The server has crashed.  Please investigate.")

    def test_404_not_found_1(self):
        """  Test Name: test_404_not_found_1\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /junk HTTP/1.1
        """
        run_404_check(self.http_connection, "/api/junk", self.hostname)

    def test_404_not_found_2(self):
        """  Test Name: test_404_not_found_2\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /api/login/api/login HTTP/1.1
        """
        run_404_check(self.http_connection, "/api/login/api/login", self.hostname)

    def test_404_not_found_3(self):
        """  Test Name: test_404_not_found_3\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /api/logon HTTP/1.1
        """
        run_404_check(self.http_connection, "/api/logon", self.hostname)

    def test_404_not_found_4(self):
        """  Test Name: test_404_not_found_4\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /api/api/login HTTP/1.1
        """
        run_404_check(self.http_connection, "/api/api/login", self.hostname)

    def test_404_not_found_5(self):
        """  Test Name: test_404_not_found_5\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /api/loginjunk HTTP/1.1
        """
        run_404_check(self.http_connection, "/api/api", self.hostname)

    def test_404_not_found_6(self):
        """  Test Name: test_404_not_found_6\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /api/loginjunk HTTP/1.1
        """
        run_404_check(self.http_connection, "/api/loginjunk", self.hostname)

    def test_404_not_found_7(self):
        """  Test Name: test_404_not_found_7\n\
        Number Connections: 1 \n\
        Procedure: Test a simple GET request for an illegal object URL:\n\
            GET /login/api HTTP/1.1
        """
        run_404_check(self.http_connection, "/login/api", self.hostname)
    def test_method_check_long(self):
        """  Test Name: test_method_check_4\n\
        Number Connections: 1 \n\
        Procedure: Test a request using a long method:\n\
            aa....aaa /api/login HTTP/1.1
        """
        run_method_check(self.http_connection, self.a_string*2, self.hostname)

    def test_method_check_4(self):
        """  Test Name: test_method_check_4\n\
        Number Connections: 1 \n\
        Procedure: Test a request using a different method than GET:\n\
            ASD /api/login HTTP/1.1
        """
        run_method_check(self.http_connection, "ASD", self.hostname)

    def test_login_post_invalid_username(self):
        """ Test Name: test_login_post_invalid_username\n
        Number Connections: One \n\
        Procedure: Simple POST request:\n\
            POST /api/login HTTP/1.1

        Run a check for login by providing an incorrect username using a
        well-formed request for /api/login. Not checking for response text.
        """
        # JSON body for the request
        data = {"username": self.invalid_username, "password": self.password}
        body = json.dumps(data)

        # POST request for login
        self.http_connection.request("POST", "/api/login", body)

        # Get the server response
        server_response = self.http_connection.getresponse()

        # Check the response code
        self.assertEqual(server_response.status, FORBIDDEN)

    def test_login_post_invalid_password(self):
        """ Test Name: test_login_post_invalid_password\n
        Number Connections: One \n\
        Procedure: Simple POST request:\n\
            POST /api/login HTTP/1.1

        Run a check for login by providing an incorrect password using a
        well-formed request for /api/login. Not checking for response text.
        """
        # JSON body for the request
        data = {"username": self.username, "password": self.invalid_password}
        body = json.dumps(data)

        # POST request for login
        self.http_connection.request("POST", "/api/login", body)

        # Get the server response
        server_response = self.http_connection.getresponse()

        # Check the response code
        self.assertEqual(server_response.status, FORBIDDEN)


    def test_login_post_invalid_body(self):
        """ Test Name: test_login_post_invalid_body\n
        Number Connections: One \n\
        Procedure: Simple POST request:\n\
            POST /api/login HTTP/1.1

        Run a check for login by providing an ill-formed body i.e. not JSON
        for /api/login. Not checking for response text.
        """
        # Ill-formed body for the request
        data = '"username": "%s", "password": "%s"' % (self.username, self.password)

        # POST request for login
        self.http_connection.request("POST", "/api/login", data)

        # Get the server response
        server_response = self.http_connection.getresponse()

        # Check the response code (403 or 400)
        self.assertTrue(server_response.status == FORBIDDEN or
                        server_response.status == BAD_REQUEST)

    def test_login_valid_body_extra_parameters(self):
        """  Test Name: test_login_valid_body_extra_parameters\n
        Number Connections: One \n\
        Procedure: Simple POST request:\n\
            POST /api/login HTTP/1.1

        Run a check for login by providing a well-formed body with extra
        parameters in the JSON body for /api/login. Not checking for
        response text.
        """
        # JSON body for the request
        data = {"username": self.username, "password": self.password, "key": "value"}
        body = json.dumps(data)

        # POST request for login
        self.http_connection.request("POST", "/api/login", body)

        # Get the server response
        server_response = self.http_connection.getresponse()

        # Check the response code
        self.assertEqual(server_response.status, OK)


class Multi_Conn_Sequential_Case(Doc_Print_Test_Case):
    """
    Test case for multiple connections, using good requests that are properly
    formed.  Further, the requests are processed sequentially.
    The tests are aptly named for describing their effects.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Multi_Conn_Sequential_Case, self).__init__(testname)
        self.hostname = hostname
        self.port = port

    def setUp(self):
        """  Test Name: None -- setUp function\n\
        Number Connections: N/A \n\
        Procedure: Opens the HTTP connection to the server.  An error here \
                   means the script was unable to create a connection to the \
                   server.
        """
        self.http_connections = []

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: Closes the HTTP connection to the server.  An error here \
                   means the server crashed after servicing the request from \
                   the previous test.
        """
        for http_conn in self.http_connections:
            http_conn.close()
        if server.poll() is not None:
            # self.fail("The server has crashed.  Please investigate.")
            print("The server has crashed.  Please investigate.")

    def test_two_connections(self):
        """  Test Name: test_two_connections\n\
        Number Connections: 2 \n\
        Procedure: Run 2 connections simultaneously for simple GET requests:\n\
            GET /api/login HTTP/1.1
        """

        # Append two connections to the list
        for x in range(2):
            self.http_connections.append(HTTPConnection(self.hostname,
                                                        self.port))
        # Connect each connection
        for http_conn in self.http_connections:
            http_conn.connect()

        # Run a request for /api/login and check it
        # we run these in opposite order so that serial server implementations
        # fail.
        for http_conn in reversed(self.http_connections):
            run_connection_check_empty_login(http_conn, self.hostname)

        # Run a request for /api/login and check it
        for http_conn in self.http_connections:
            run_connection_check_empty_login(http_conn, self.hostname)



    def test_four_connections(self):
        """  Test Name: test_two_connections\n\
        Number Connections: 4 \n\
        Procedure: Run 4 connections simultaneously for simple GET requests:\n\
            GET /api/login HTTP/1.1
        """

        # Append four connections to the list
        for x in range(4):
            self.http_connections.append(HTTPConnection(self.hostname,
                                                        self.port))
        # Connect each connection
        for http_conn in self.http_connections:
            http_conn.connect()

        # Run a request for /api/login and check it
        for http_conn in reversed(self.http_connections):
            run_connection_check_empty_login(http_conn, self.hostname)

        # Run a request for /api/login and check it
        for http_conn in self.http_connections:
            run_connection_check_empty_login(http_conn, self.hostname)

    def test_eight_connections(self):
        """  Test Name: test_two_connections\n\
        Number Connections: 8 \n\
        Procedure: Run 8 connections simultaneously for simple GET requests:\n\
            GET /api/login HTTP/1.1
        """

        # Append eight connections to the list
        for x in range(8):
            self.http_connections.append(HTTPConnection(self.hostname,
                                                        self.port))
        # Connect each connection
        for http_conn in self.http_connections:
            http_conn.connect()

        # Run a request for /api/login and check it
        for http_conn in reversed(self.http_connections):
            run_connection_check_empty_login(http_conn, self.hostname)

        # Re-connect in the case of HTTP/1.0 protocol implementation
        for http_conn in self.http_connections:
            http_conn.connect()

        # Run a request for /api/login and check it
        for http_conn in self.http_connections:
            run_connection_check_empty_login(http_conn, self.hostname)



class Single_Conn_Good_Case(Doc_Print_Test_Case):
    """
    Test case for a single connection, using good requests that are properly
    formed.  The tests are aptly named for describing their effects.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Single_Conn_Good_Case, self).__init__(testname)

        self.hostname = hostname
        self.port = port

    def setUp(self):
        """  Test Name: None -- setUp function\n\
        Number Connections: N/A \n\
        Procedure: Opens the HTTP connection to the server.  An error here \
                   means the script was unable to create a connection to the \
                   server.
        """
        # Make HTTP connection for the server
        self.http_connection = HTTPConnection(self.hostname, self.port)

        # Connect to the server
        self.http_connection.connect()

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: Closes the HTTP connection to the server.  An error here \
                   means the server crashed after servicing the request from \
                   the previous test.
        """
        # Close the HTTP connection
        self.http_connection.close()
        if server.poll() is not None:
            # self.fail("The server has crashed.  Please investigate.")
            print("The server has crashed.  Please investigate.")

    def test_login_get(self):
        """  Test Name: test_loadavg_no_callback\n\
        Number Connections: One \n\
        Procedure: Simple GET request:\n\
            GET /api/login HTTP/1.1
        """

        # GET request for the object /loadavg
        self.http_connection.request("GET", "/api/login")

        # Get the server's response
        server_response = self.http_connection.getresponse()

        # Check the response status code
        self.assertEqual(server_response.status, OK, "Server failed to respond")

        # Check the data included in the server's response
        self.assertTrue(check_empty_login_respnse(server_response.read().decode('utf8')), \
                        "login check failed")


class Access_Control(Doc_Print_Test_Case):
    """
    Test cases for access control, using good requests that are properly
    formed. The tests are aptly named for describing their effects.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Access_Control, self).__init__(testname)

        self.hostname = hostname
        self.port = port
        self.public_file_1 = 'index.html'
        self.public_file_2 = 'js/jquery.min.js'
        self.public_file_3 = 'css/jquery-ui.min.css'
        self.private_file = 'private/secure.html'
        self.username = 'user0'
        self.password = 'thepassword'
        self.invalid_password = 'wrongpassword'

    def setUp(self):
        """  Test Name: None -- setUp function\n\
        Number Connections: N/A \n\
        Procedure: Opens the HTTP connection to the server.  An error here \
                   means the script was unable to create a connection to the \
                   server.
        """
        # Create a requests session
        self.session = requests.Session()

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: Closes the HTTP connection to the server.  An error here \
                   means the server crashed after servicing the request from \
                   the previous test.
        """
        # Close the HTTP connection
        self.session.close()

    def test_access_control_private_valid_token(self):
        """ Test Name: test_access_control_private_valid_token
        Number Connections: N/A
        Procedure: Checks if private files can be accessed given the right
                   username and password. An error here means that the either
                   the server did not authenticate the user correctly or that
                   despite being authenticated the user is not served with
                   the contents of the private path.
        """
        # Login using the default credentials
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'username': self.username, 'password': self.password},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is authenticated
        self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        response = self.session.get(url, timeout=2)

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with private file despite being authenticated.")
    
    def test_access_control_public_valid_token(self):
        """ Test Name: test_access_control_public_valid_token
        Number Connections: N/A
        Procedure: Checks if public files can be accessed given the right
                   username and password. Public paths DO NOT require a
                   username and password. A failure here means that
                   authentication failed or that public paths are not being
                   served.
        """
        # Login using the default credentials
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'username': self.username, 'password': self.password},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is authenticated
        self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

        # Define the public URL to get - test HTML file
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.public_file_1)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with public file despite being authenticated.")

        # Ensure that file was correctly returned
        self.assertEqual(response.content, open(f'{base_dir}/{self.public_file_1}', 'rb').read())

        # Define the public URL to get - test JS file
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.public_file_2)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with public file despite being authenticated.")

        # Ensure that file was correctly returned
        self.assertEqual(response.content, open(f'{base_dir}/{self.public_file_2}', 'rb').read())

        # Define the public URL to get - test CSS file
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.public_file_3)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with public file despite being authenticated.")

        # Ensure that file was correctly returned
        self.assertEqual(response.content, open(f'{base_dir}/{self.public_file_3}', 'rb').read())
                
    def test_access_control_public_no_token(self):
        """ Test Name: test_access_control_public_no_token
        Number Connections: N/A
        Procedure: Checks if public files can be accessed without a username
                   and password. A failure here means that public paths are
                   not being served.
        """
        # Define the public URL to get - test HTML file
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.public_file_1)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with a public file.")

        # Define the public URL to get - test JS file
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.public_file_1)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with a public file.")

        # Define the public URL to get - test CSS file
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.public_file_1)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is granted
        self.assertEqual(response.status_code, requests.codes.ok,
                         "Server failed to respond with a public file.")
                        
    def test_access_control_private_invalid_token(self):
        """ Test Name: test_access_control_private_invalid_token
        Number Connections: N/A
        Procedure: Checks if private files can be accessed given invalid
                   authentication details. A failure here means that
                   authentication succeeded or that private paths are being
                   served without authentication.
        """
        # Login using the default credentials
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'username': self.username, 'password': self.invalid_password},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is authenticated
        self.assertEqual(response.status_code, requests.codes.forbidden, "Authentication failed.")

        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with private file despite not being authenticated.")
                         
    def test_access_control_private_valid_semantic_token(self):
        """ Test Name: test_access_control_private_valid_semantic_token
        Number Connections: N/A
        Procedure: Checks if JSON parsing appropriately guards against
                   missing key/value pairs in the request body (e.g. a 
                   request without "username" or "password".)
                   The JSON might be semantically valid, but not
                   hold the requisite key/value pairs that are needed.
        """
        # Login using the default credentials
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'foo': 'bar'},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is not authenticated
        # accepted responses include 400 and 403
        if response.status_code not in [requests.codes.bad_request, requests.codes.forbidden]:
            raise AssertionError("Server did not respond with 403 or 400 to missing authentication info")

        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with private file despite not being authenticated.")    
    
    def test_access_control_private_no_token(self):
        """ Test Name: test_access_control_private_no_token
        Number Connections: N/A
        Procedure: Checks if private files can be accessed without
                   authentication details. A failure here means that
                   private paths are being served without authentication.
        """
        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with a private file despite no token.")

    def test_access_control_private_malformed_token(self):
        """ Test Name: test_access_control_private_no_token
        Number Connections: N/A
        Procedure: Checks if private files can be accessed with malformed
                   authentication tokens. A failure here means that the
                   authentication token is not being checked when serving
                   private files.
        """
        # Login using the default credentials
        try:
            response = self.session.post('http://%s:%s/api/login' % (self.hostname, self.port),
                                         json={'username': self.username, 'password': self.password},
                                         timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that the user is authenticated
        self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

        # Get cookie name
        for cookie in self.session.cookies:
            try:
                encoded_data = cookie.value.split('.')[1]

                # Try to decode the payload
                decoded_payload = decode_base64(encoded_data)

                # Get decoded_payload as JSON
                data = json.loads(decoded_payload)

                break

            except (IndexError, ValueError):
                continue

        # Delete the current authentication token
        del self.session.cookies[cookie.name]

        # Set a false authentication token in the cookie
        self.session.cookies.set(cookie.name, 'false_token')

        # Define the private URL to get
        url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        try:
            response = self.session.get(url, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        # Ensure that access is forbidden
        self.assertEqual(response.status_code, requests.codes.forbidden,
                         "Server responded with private file despite given an invalid auth token.")

    def test_access_control_private_path(self):
        """ Test Name: test_access_control_private_path
        Number Connections: N/A
        Procedure: Checks if private files can be accessed through redirection.
                   A failure here means that the private paths are not
                   adequately protected and paths such as /public/../private/secret
                   can be accessed.
        """
        # Define the private URL to get prefixed with a public path
        url = 'http://%s:%s/public/../%s' % (self.hostname, self.port, self.private_file)

        # Use the session cookie to get the private file
        try:
            # prevent path segment normalization
            req = requests.Request('GET', url)
            prepared_req = req.prepare()
            # set the URL manually in case newer requests/urllib3 normalizes URL
            # in prepare() function
            prepared_req.url = url
            response = self.session.send(prepared_req, timeout=2)
        except requests.exceptions.RequestException:
            raise AssertionError("The server did not respond within 2s")

        if (response.status_code == requests.codes.forbidden):
            raise AssertionError('Server responded with 403 FORBIDDEN instead of 404 NOT FOUND')

        # Ensure that response code is 404
        self.assertEqual(response.status_code, requests.codes.not_found,
                         "Server did not respond with 404 when it should have, possible IDOR?")


class Authentication(Doc_Print_Test_Case):
    """
    Test cases for authentication expiry, using good requests that are properly
    formed. The tests are aptly named for describing their effects.
    """

    def __init__(self, testname, hostname, port):
        """
        Prepare the test case for creating connections.
        """
        super(Authentication, self).__init__(testname)

        self.hostname = hostname
        self.port = port
        self.public_file = 'index.html'
        self.private_file = 'private/secure.html'
        self.username = 'user0'
        self.password = 'thepassword'
        self.incorrect_password = 'wrongword'
        self.sleep_time = 8 if run_slow else 4
        self.current_year = datetime.now().year

    def setUp(self):
        """  Test Name: None -- setUp function\n\
        Number Connections: N/A \n\
        Procedure: Opens the HTTP connection to the server.  An error here \
                   means the script was unable to create a connection to the \
                   server.
        """
        # Create a requests session
        self.sessions = []

    def tearDown(self):
        """  Test Name: None -- tearDown function\n\
        Number Connections: N/A \n\
        Procedure: Closes the HTTP connection to the server.  An error here \
                   means the server crashed after servicing the request from \
                   the previous test.
        """
        del self.sessions

    def test_expires_authentication_token(self):
        """ Test Name: test_expires_authentication_token
        Number Connections: 30
        Procedure: Checks if the authentication token expires in the timeframe
                   given. An error here means that expiry authentication token
                   duration is not correctly configured.
        """
        # Create multiple sessions
        for i in range(30):
            self.sessions.append(requests.Session())

        # Randomize failure events
        should_fail = lambda: bool(random.getrandbits(1))

        def test_expiry_authentication(i):
            # Login using the default credentials
            try:
                response = self.sessions[i].post('http://%s:%s/api/login' % (self.hostname, self.port),
                                             json={'username': self.username, 'password': self.password},
                                             timeout=2)
            except requests.exceptions.RequestException:
                raise AssertionError("The server did not respond within 2s POST")

            # Ensure that the user is authenticated
            self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

            # Define the private URL to get
            url = 'http://%s:%s/%s' % (self.hostname, self.port, self.private_file)

            if should_fail():
                # Use the session cookie to get the private file
                try:
                    response = self.sessions[i].get(url, timeout=2)
                except requests.exceptions.RequestException:
                    raise AssertionError("The server did not respond within 2s GET %s" % self.private_file)

                # Ensure that access is granted
                self.assertEqual(response.status_code, requests.codes.ok,
                                 "Server did not respond with the private file despite valid authentication.")
            else:
                # Sleep for a short duration till token expires
                time.sleep(self.sleep_time)

                # Use the session cookie to get the private file
                try:
                    response = self.sessions[i].get(url, timeout=2)
                except requests.exceptions.RequestException:
                    raise AssertionError("The server did not respond within 2s")

                # Ensure that response is not OK
                assert response.ok == False, "The response to the query was 200 OK when it should have been 403 FORBIDDEN"

                # Ensure that access is forbidden
                self.assertEqual(response.status_code, requests.codes.forbidden,
                                 "Server responded with %s (expected value = %s) when the token expires."
                                    % (response.status_code, requests.codes.forbidden))

            self.sessions[i].close()

        pool = ThreadPool(30)
        pool.map(test_expiry_authentication, range(30))
        pool.terminate()

    def test_jwt_claims_json(self):
        """ Test Name: test_jwt_claims_json
        Number Connections: N/A
        Procedure: Checks if the JWT JSON has the right claims set.
                   An error here means that some of the claims required are
                   not being set correctly.
        """
        # Create multiple sessions
        for i in range(30):
            self.sessions.append(requests.Session())

        for i in range(30):
            # Login using the default credentials
            try:
                response = self.sessions[i].post('http://%s:%s/api/login' % (self.hostname, self.port),
                                             json={'username': self.username, 'password': self.password},
                                             timeout=2)
            except requests.exceptions.RequestException:
                raise AssertionError("The server did not respond within 2s")

            # Ensure that the user is authenticated
            self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

            try:
                # Convert the response to JSON
                data = response.json()

                # Verify that the JWT contains 'iat'
                assert 'iat' in data, "Could not find the claim 'iat' in the JSON object."

                # Verify that the JWT contains 'iat'
                assert 'exp' in data, "Could not find the claim 'exp' in the JSON object."

                # Verify that the JWT contains 'sub'
                assert 'sub' in data, "Could not find the claim 'sub' in the JSON object."

                # Verify that the 'iat' claim to is a valid date from self.current_year
                assert datetime.fromtimestamp(data['iat']).year == self.current_year, "'iat' returned is not a valid date"

                # Verify that the 'exp' claim to is a valid date from self.current_year
                assert datetime.fromtimestamp(data['exp']).year == self.current_year, "'exp' returned is not a valid date"

                # Verify that the subject claim to is set to the right username
                assert data['sub'] == self.username, "The subject claim 'sub' should be set to %s" % self.username

            except ValueError:
                raise AssertionError('The login API did not return a valid JSON object')

            # Sleep for a short duration before testing again
            time.sleep(random.random() / 10.0)

            # Close the session
            self.sessions[i].close()

    def test_jwt_claims_cookie(self):
        """ Test Name: test_jwt_claims_cookie
        Number Connections: N/A
        Procedure: Checks if the JWT cookie has the right claims set.
                   An error here means that some of the claims required are
                   not being set correctly.
        """
        # Create multiple sessions
        for i in range(30):
            self.sessions.append(requests.Session())

        for i in range(30):
            # Login using the default credentials
            try:
                response = self.sessions[i].post('http://%s:%s/api/login' % (self.hostname, self.port),
                                             json={'username': self.username, 'password': self.password},
                                             timeout=2)
            except requests.exceptions.RequestException:
                raise AssertionError("The server did not respond within 2s")

            # Ensure that the user is authenticated
            self.assertEqual(response.status_code, requests.codes.ok, "Authentication failed.")

            # Get the cookie value from the response
            found_cookie = False

            for cookie in self.sessions[i].cookies:
                try:
                    encoded_data = cookie.value.split('.')[1]

                    # Try to decode the payload
                    decoded_payload = decode_base64(encoded_data)

                    # Get decoded_payload as JSON
                    data = json.loads(decoded_payload)

                    found_cookie = True

                except (IndexError, ValueError):
                    continue

            # If cookie is None, it means no cookie has been set
            if not found_cookie:
                raise AssertionError('No valid cookie found.')

            # Verify that the JWT contains 'iat'
            assert 'iat' in data, "Could not find the claim 'iat' in the JSON object."

            # Verify that the JWT contains 'iat'
            assert 'exp' in data, "Could not find the claim 'exp' in the JSON object."

            # Verify that the JWT contains 'sub'
            assert 'sub' in data, "Could not find the claim 'sub' in the JSON object."

            # Verify that the 'iat' claim to is a valid date from self.current_year
            assert datetime.fromtimestamp(data['iat']).year == self.current_year, "'iat' returned is not a valid date"

            # Verify that the 'exp' claim to is a valid date from self.current_year
            assert datetime.fromtimestamp(data['exp']).year == self.current_year, "'exp' returned is not a valid date"

            # Verify that the subject claim to is set to the right username
            assert data['sub'] == self.username, "The subject claim 'sub' should be set to %s" % self.username

            # Sleep for a short duration before testing again
            time.sleep(random.random() / 10.0)

            # Close the session
            self.sessions[i].close()

###############################################################################
# Globally define the Server object so it can be checked by all test cases
###############################################################################
server = None
output_file_name = None

from signal import SIGKILL
def killserver(server):
    pid = server.pid
    try:
        pgid = os.getpgid(pid)
        os.killpg(pgid, SIGKILL)
    except OSError:
        # process might already be dead, os.getpgid throw in this case
        pass

###############################################################################
# Define an atexit shutdown method that kills the server as needed
###############################################################################
def make_clean_up_testing(server):
    def clean_up_testing():
        try:
            killserver(server)   # SIGKILL
        except:
            pass

    return clean_up_testing


# Grade distribution constants
grade_points_available = 95
# 6 tests
minreq_total = 25
# 27 tests
extra_total = 20
# 5 tests
malicious_total = 20
# 4 tests
ipv6_total = 5
# ? tests
auth_total = 25


def print_points(minreq, extra, malicious, ipv6, auth):
    """All arguments are fractions (out of 1)"""
    print("Minimum Requirements:         \t%2d/%2d" % (int(minreq * minreq_total), minreq_total))
    print("Authentication Functionality: \t%2d/%2d" % (int(auth * auth_total), auth_total))
    print("IPv6 Functionality:           \t%2d/%2d" % (int(ipv6 * ipv6_total), ipv6_total))
    print("Extra Tests:                  \t%2d/%2d" % (int(extra * extra_total), extra_total))
    print("Robustness:                   \t%2d/%2d" % (int(malicious * malicious_total), malicious_total))


###############################################################################
# Main
###############################################################################
# Not sure if this is necessary
if __name__ == '__main__':

    try:
        opts, args = getopt.getopt(sys.argv[1:], "ndhs:t:o:l6:w", \
                                   ["help"])
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err))  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    server_path = None
    run_slow = False
    individual_test = None
    runIPv6 = True
    list_tests = False
    ipv6_host = "localhost6"

    ipv6_score = 0
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-s"):
            server_path = a
        elif o in ("-t"):
            individual_test = a
        elif o in ("-l"):
            list_tests = True
        elif o in ("-w"):
            run_slow = True
        elif o in ("-o"):
            output_file_name = a
        elif o in ("-6"):
            ipv6_host = a
        else:
            assert False, "unhandled option"

    alltests = [Single_Conn_Good_Case, Multi_Conn_Sequential_Case, Single_Conn_Bad_Case,
                Single_Conn_Malicious_Case, Single_Conn_Protocol_Case, Access_Control,
                Authentication]


    def findtest(tname):
        for clazz in alltests:
            if tname in dir(clazz):
                return clazz
        return None


    if list_tests:
        for clazz in alltests:
            print("In:", clazz.__name__)
            for test in [m for m in dir(clazz) if m.startswith("test_")]:
                print("\t", test)

        sys.exit(0)

    if server_path is None:
        usage()
        sys.exit()

    # Check access to the server path
    if not os.access(server_path, os.R_OK):
        print("File ", server_path, " is not readable")
        sys.exit(1)

    # Setting the default timeout to allow connections to time out
    socket.setdefaulttimeout(4)

    # Determine the hostname for running the server locally
    hostname = socket.gethostname()

    # Determine the port number to use, based off of the current PID.
    port = (os.getpid() % 10000) + 20000

    # Set the base directory for the server, relative to
    # where this script is located
    base_dir = f'{script_dir}/test_root_data'

    # Authentication token expiry
    auth_token_expiry = '2'

    def start_server(preargs = [], postargs = []):
        args = preargs + [server_path, "-p", str(port), "-R", base_dir] + postargs
        output_file = None

        # Open the output file if possible
        if output_file_name is not None:
            output_file = open(output_file_name, "a")

        def make_new_pgrp():
            os.setpgid(0, 0)

        if output_file is not None:
            # Open the server on this machine
            server = subprocess.Popen(args, preexec_fn = make_new_pgrp,
                                      stdout=output_file, stderr=subprocess.STDOUT)
        else:
            server = subprocess.Popen(args, preexec_fn = make_new_pgrp)

        # Register the atexit function to shutdown the server on Python exit
        atexit.register(make_clean_up_testing(server))

        return server

    server = start_server()

    # Ensure that the server is running and accepting connections.
    counter = 0
    while True:
        try:
            # by using the IP address returned here, we force the use of IPv4
            localhostname = socket.gethostbyname(socket.gethostname())
            http_conn = HTTPConnection(localhostname, port)
            http_conn.connect()
            http_conn.close()
            break
        except:
            if counter >= 10:
                print("""
The server is not responding to IPv4 connection requests, and may not be
functioning properly.  Ensure that you sent the proper location for your
server, and that your server starts running in a reasonable amount of time
(this waited 5 seconds for your server to start running).

In the case that your server works fine and there's an error in our
script, please use the 'ps' command to see if the server is still running,
and let us know if there is an issue with our script creating a runaway
process.
                """)
                sys.exit(1)

            counter += 1
            time.sleep(.5)

    print("Your server has started successfully.  Now to begin testing.")
    # If an individual test was requested, find that test and only add it.  If no
    # tests are found of that name, error and exit.
    print('should be running one test')
    if individual_test is not None:
        single_test_suite = unittest.TestSuite()
        testclass = findtest(individual_test)
        if testclass == Authentication:
            killserver(server)
            server.wait()
            server = start_server(postargs=['-e', auth_token_expiry])
            time.sleep(3 if run_slow else 1)
        if testclass:
            single_test_suite.addTest(testclass(individual_test, hostname, port))
        else:
            print("The test \"" + individual_test + "\" was not found in the test classes. Use -l.")
            sys.exit(1)

        # Run the single test test suite and store the results
        test_results = unittest.TextTestRunner().run(single_test_suite)

        if test_results.wasSuccessful():
            print("Test: " + individual_test + " passed!")
        else:
            print("Test: " + individual_test + " failed.")

    else:

        # Test Suite for the minimum requirements
        min_req_suite = unittest.TestSuite()

        # Add all of the tests from the class Single_Conn_Good_Case
        for test_function in dir(Single_Conn_Good_Case):
            if test_function.startswith("test_"):
                min_req_suite.addTest(Single_Conn_Good_Case(test_function, hostname, port))

        # In particular, add the two-connection test from Multi_Conn_Sequential_Case,
        # and the 1.0 protocol check (early return check) from Single_Conn_Protocol_Case
        min_req_suite.addTest(Multi_Conn_Sequential_Case("test_two_connections", hostname, port))
        min_req_suite.addTest(Single_Conn_Protocol_Case("test_http_1_0_compliance", hostname, port))

        # Test Suite to test JWT/authentication functionality
        auth_tests_suite = unittest.TestSuite()

        # Add all of the tests from the class Access_Control
        for test_function in dir(Access_Control):
            if test_function.startswith("test_"):
                auth_tests_suite.addTest(Access_Control(test_function, hostname, port))

        # Add all of the tests from the class Authentication
        for test_function in dir(Authentication):
            if test_function.startswith("test_"):
                auth_tests_suite.addTest(Authentication(test_function, hostname, port))

        # Test Suite for extra points, mostly testing error cases
        extra_tests_suite = unittest.TestSuite()

        # Add all of the tests from the class Multi_Conn_Sequential_Case
        for test_function in dir(Multi_Conn_Sequential_Case):
            if test_function.startswith("test_"):
                extra_tests_suite.addTest(Multi_Conn_Sequential_Case(test_function, hostname, port))

        # Add all of the tests from the class Single_Conn_Bad_Case
        for test_function in dir(Single_Conn_Bad_Case):
            if test_function.startswith("test_"):
                extra_tests_suite.addTest(Single_Conn_Bad_Case(test_function, hostname, port))

        # In particular, add the 1.1 protocol persistent connection check from Single_Conn_Protocol_Case
        extra_tests_suite.addTest(Single_Conn_Protocol_Case("test_http_1_1_compliance", hostname, port))

        # Malicious Test Suite
        malicious_tests_suite = unittest.TestSuite()

        # Add all of the tests from the class Single_Conn_Malicious_Case
        for test_function in dir(Single_Conn_Malicious_Case):
            if test_function.startswith("test_"):
                malicious_tests_suite.addTest(Single_Conn_Malicious_Case(test_function, hostname, port))

        print('Beginning the Minimum Requirement Tests')
        time.sleep(3 if run_slow else 1)
        # Run the minimum requirements test suite and store the results
        test_results = unittest.TextTestRunner().run(min_req_suite)

        nt = min_req_suite.countTestCases()
        minreq_score = max(0, F(nt - len(test_results.errors) - len(test_results.failures), nt))

        # Check if the server passed the minimum requirements
        if test_results.wasSuccessful():
            print("\nYou have passed the Minimum Requirements for this project!\n")
        else:
            print("\nYou have NOT passed the Minimum Requirements for this project.\n" +
                  "Please examine the above errors, the remaining tests\n" +
                  "will not be run until after the above tests pass.\n")

            print_points(minreq_score, 0, 0, 0, 0)
            sys.exit()

        print('Beginning Authentication Tests')
        # Kill the server and start it again with expiry flag set to auth_token_expiry seconds
        killserver(server)
        server.wait()
        server = start_server(postargs=['-e', auth_token_expiry])

        time.sleep(3 if run_slow else 1)
        # Run the extra tests
        test_results = unittest.TextTestRunner().run(auth_tests_suite)

        auth_score = max(0,
                          F(auth_tests_suite.countTestCases() - len(test_results.errors) - len(test_results.failures),
                            auth_tests_suite.countTestCases()))

        def makeTestSuiteForHost(hostname):
            # IPv6 Test Suite
            ipv6_test_suite = unittest.TestSuite()
            # Add all of the tests from the class Single_Conn_Good_Case
            for test_function in dir(Single_Conn_Good_Case):
                if test_function.startswith("test_"):
                    ipv6_test_suite.addTest(Single_Conn_Good_Case(test_function, hostname, port))

            return ipv6_test_suite

        if runIPv6:
            #
            # Now run IPv6 in various combinations
            #
            # check that base server can accept IPv6 connections.
            ts1 = makeTestSuiteForHost(ipv6_host)
            testcases, points = 0, 0

            def run_and_count(msg, ts1):
                global testcases, points
                print (msg)
                test_results = unittest.TextTestRunner().run(ts1)
                testcases += ts1.countTestCases()
                points += ts1.countTestCases() - len(test_results.errors) - len(test_results.failures)

            run_and_count("Checking that server can accept IPv6 connections", ts1)

            def restart_server(preargs=args):
                killserver(server)
                server.wait()
                newserver = start_server(preargs=preargs)
                time.sleep(3 if run_slow else 1)
                return newserver

            server = restart_server(preargs=['env', 'REVERSEIPADDR=1', f'LD_PRELOAD={ld_preload}'])

            ts2 = makeTestSuiteForHost(ipv6_host)
            run_and_count("Checking that server can accept IPv6 connections if addresses are in reverse", ts2)

            # check that server can accept IPv6 connections if only IPv6 addresses are listed
            server = restart_server(preargs=['env', 'SKIPIPV4=1', f'LD_PRELOAD={ld_preload}'])

            ts3 = makeTestSuiteForHost(ipv6_host)
            run_and_count("Checking that server can accept IPv6 connections if no IPv4 addresses", ts3)

            # check that server can accept IPv4 connections if only IPv4 addresses are listed
            server = restart_server(preargs=['env', 'SKIPIPV6=1', f'LD_PRELOAD={ld_preload}'])

            ts4 = makeTestSuiteForHost(hostname)
            run_and_count("Checking that server can accept IPv4 connections if no IPv6 addresses", ts4)

            ipv6_score = max(0, F(points, testcases))
#
            if points == testcases:
                print("\nCongratulations! IPv6 support appears to work!\n")
            else:
                print(
                        "\nYou have NOT passed the IPv6 portion.  Check that your code is properly handles all possible configurations.  " +
                        "Please examine the errors listed above.\n")

        print('Beginning Extra Tests')
        server = restart_server()
        time.sleep(3 if run_slow else 1)
        # Run the extra tests
        test_results = unittest.TextTestRunner().run(extra_tests_suite)

        extra_score = max(0,
                          F(extra_tests_suite.countTestCases() - len(test_results.errors) - len(test_results.failures),
                            extra_tests_suite.countTestCases()))

        # Kill the server and start it normally without the expiry set to auth_token_expiry seconds
        killserver(server)
        server.wait()
        server = start_server()

        # Check if the server passed the extra tests
        if test_results.wasSuccessful():
            print("\nYou have passed the Extra Tests for this project!\n")
        else:
            print("\nYou have NOT passed the Extra Tests for this project.\n" +
                  "Please examine the above errors, the Malicious Tests\n" +
                  "will not be run until the above tests pass.\n")

            print_points(minreq_score, extra_score, 0, ipv6_score, auth_score)
            sys.exit()

        print("Now running the MALICIOUS Tests.  WARNING:  These tests will not necessarily run fast!")
        time.sleep(1)
        # Run the malicious tests
        test_results = unittest.TextTestRunner().run(malicious_tests_suite)

        robustness_score = max(0, F(
            malicious_tests_suite.countTestCases() - len(test_results.errors) - len(test_results.failures),
            malicious_tests_suite.countTestCases()))

        # Check if the server passed the extra tests
        if test_results.wasSuccessful():
            print("\nCongratulations! You have passed the Malicious Tests!\n")
        else:
            print("\nYou have NOT passed one or more of the Malicious Tests.  " +
                  "Please examine the errors listed above.\n")

        print_points(minreq_score, extra_score, robustness_score, ipv6_score, auth_score)

