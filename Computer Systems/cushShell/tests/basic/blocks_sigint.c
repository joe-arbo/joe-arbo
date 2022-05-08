#include <signal.h>
#include <unistd.h>

int
main()
{
    signal(SIGINT, SIG_IGN);
    sleep(100);
}
