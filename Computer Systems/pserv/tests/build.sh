# build the getaddrinfo preloaded library to test IPv4/IPv6 scenarios
gcc -fPIC -Wall -c getaddrinfo.c
gcc -shared -o getaddrinfo.so.1.0.1 getaddrinfo.o -ldl

