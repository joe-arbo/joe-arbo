/*
 * Intercept getaddrinfo to simulate IPv4-only, IPv6-only, and
 * environment in which addresses are returned in a different order.
 *
 * Written for CS 3214 Virginia Tech, Spring 2018.
 *
 * Godmar Back <godmar@gmail.com>
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#define __USE_GNU 1     /* for RTLD_NEXT */
#include <dlfcn.h>

static int (*real_getaddrinfo)(const char *node, 
        const char *service,
        const struct addrinfo *hints,
        struct addrinfo **res);

static void (*real_freeaddrinfo)(struct addrinfo *res);

static void
init()
{
    static int inited;

    if (inited++ > 0) {
        return;
    }

    real_getaddrinfo = dlsym(RTLD_NEXT, "getaddrinfo");
    if (!real_getaddrinfo) {
        printf("error in dlsym getaddrinfo %s\n", dlerror());
        exit(-1);
    }

    real_freeaddrinfo = dlsym(RTLD_NEXT, "freeaddrinfo");
    if (!real_freeaddrinfo) {
        printf("error in dlsym freeaddrinfo %s\n", dlerror());
        exit(-1);
    }
}

int getaddrinfo(
        const char *node, 
        const char *service,
        const struct addrinfo *hints,
        struct addrinfo **res)
{
    init();
     
    struct addrinfo * reallist;
    int rc = real_getaddrinfo(node, service, hints, &reallist);
    if (rc != 0)
        return rc;

    int skipipv4 = getenv("SKIPIPV4") != NULL;
    int skipipv6 = getenv("SKIPIPV6") != NULL;
    int reverse = getenv("REVERSEIPADDR") != NULL;
    if (reverse) {
        struct addrinfo * rp = reallist;
        struct addrinfo * last = NULL;
        while (rp != NULL) {
            struct addrinfo * next = rp->ai_next;
            rp->ai_next = last;
            last = rp;
            rp = next;
        }
        *res = last;
    } else if (skipipv4 || skipipv6) {
        struct addrinfo * rp, ** last = &reallist;
        for (rp = reallist; rp != NULL; ) {
            if ((skipipv4 && rp->ai_family == AF_INET) || (skipipv6 && rp->ai_family == AF_INET6)) {
                // skip and free
                (*last) = rp->ai_next;
                struct addrinfo * old = rp;
                rp = rp->ai_next;
                old->ai_next = NULL;
                real_freeaddrinfo(old);
            } else {
                // include
                last = &rp->ai_next;
                rp = rp->ai_next;
            }
        }
        *res = reallist;
    } else {
        *res = reallist;
    }

    return rc;
}

void freeaddrinfo(struct addrinfo *res)
{
    init();
    real_freeaddrinfo(res);
}
