#ifndef __CONFIG_H_
#define __CONFIG_H_

/*
 * config.h - malloc lab configuration file
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */

/*
 * This is the default path where the driver will look for the
 * default tracefiles. You can override it at runtime with the -t flag.
 */
#define TRACEDIR "/home/courses/cs3214/malloclab/traces/"

/*
 * This is the list of default tracefiles in TRACEDIR that the driver
 * will use for testing. Modify this if you want to add or delete
 * traces from the driver's test suite. For example, if you don't want
 * your students to implement realloc, you can delete the last two
 * traces.
 */
#define DEFAULT_TRACEFILES \
  "amptjp-bal.rep",\
  "cccp-bal.rep",\
  "cp-decl-bal.rep",\
  "expr-bal.rep",\
  "coalescing-bal.rep",\
  "random-bal.rep",\
  "random2-bal.rep",\
  "binary-bal.rep",\
  "binary2-bal.rep",\
  "realloc-bal.rep",\
  "realloc2-bal.rep"

/*
 * This constant gives the estimated performance of the libc malloc
 * package using our traces on some reference system, typically the
 * same kind of system the students use. Its purpose is to cap the
 * contribution of throughput to the performance index. Once the
 * students surpass the AVG_LIBC_THRUPUT, they get no further benefit
 * to their score.  This deters students from building extremely fast,
 * but extremely stupid malloc packages.
 *
 * gback@cs.vt.edu/spruett3@vt.edu: I set this to a value that is achieved by a r/b 
 * tree-based implementation on our rlogin cluster as of Fall 2015; 
 * regardless of the speed of the actual libc. Updated in Fall 2015.
 *
 * gback: Again updated in Spring 2016 after switching to clock.
 * This is a base line for a single-threaded implementation, without
 * locking overhead, when clock() is used to time it.
 * This is not meaningful for the multi-threaded implementation
 *
 * Updated Summer 2020 for new hardware
 * Changed Spring 2021 to reduce required tuning
 */
#define AVG_LIBC_THRUPUT      30000E3

 /* 
  * This constant determines the contributions of space utilization
  * (UTIL_WEIGHT) and throughput (1 - UTIL_WEIGHT) to the performance
  * index.  
  */
#define UTIL_WEIGHT .60

/* 
 * Alignment requirement in bytes
 * (16 for 64-bit allocators)
 */
#define ALIGNMENT 16

/* 
 * Maximum heap size in bytes 
 */
#define MAX_HEAP (1024*(1<<20))  /* 1024 MB */

/*****************************************************************************
 * Set exactly one of these USE_xxx constants to "1" to select a timing method
 *****************************************************************************/
#define USE_FCYC   0   /* cycle counter w/K-best scheme (x86 & Alpha only) */
#define USE_ITIMER 0   /* interval timer (any Unix box) */
#define USE_GETTOD 0   /* gettimeofday (any Unix box) */
#define USE_CLOCK  1   /* clock_gettime (Linux only) */

#endif /* __CONFIG_H */
