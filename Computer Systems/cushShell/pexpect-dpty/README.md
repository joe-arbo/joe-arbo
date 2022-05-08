# Pexpect

This directory contains a customized version of pexpect that uses multiple threads.
Specifically, a separate thread reads from the pseudo terminal in parallel.
This drastically simplifies writing pexpect-based tests.

Also include is the shellio module that is used by some tests.
