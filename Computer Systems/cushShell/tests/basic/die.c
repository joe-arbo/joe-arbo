#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int ac, char *av[])
{
    if (ac == 1) {
        sleep(1000);
    } else if (!strcmp("-segfault", av[1])) {
        volatile int i = *(int *)NULL;
    } else if (!strcmp("-abort", av[1])) {
        abort();
    } else if (!strcmp("-divzero", av[1])) {
        volatile int i = ac/0;
    }
}
