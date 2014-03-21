#include <signal.h>

int main()
{
    kill(3304, SIGINT);
    return 0;
}