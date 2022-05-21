#define _GNU_SOURCE
#include <spawn.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "spawn_int.h"

typedef int (*posix_spawnp_fun_t) (pid_t *pid, const char *file,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp,
                char *const argv[], char *const envp[]);

int posix_spawnp(pid_t *pid, const char *file,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *attrp,
                char *const argv[], char *const envp[])
{
    // printf("SPAWNING IT\n");
    /*
    posix_spawnp_fun_t ps = dlsym(RTLD_NEXT, "posix_spawnp");
    return ps(pid, file, file_actions, attrp, argv, envp);
    */
    return __spawni(pid, file, file_actions, attrp, argv, envp, SPAWN_XFLAGS_USE_PATH);
}

