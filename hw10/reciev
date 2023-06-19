#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"

int d = 0;
int i = 0;

pid_t sender_pid;

void sighandler(int nsig) {
    if (nsig == SIGUSR1) {
        d = d | (0 << i);
        ++i;
    } else if (nsig == SIGUSR2) {
        d = d | (1 << i);
        ++i;
    } else if (nsig == SIGINT) {
        printf("got %d\n", d);
        exit(0);
    }
    kill(sender_pid, SIGUSR2);
}

int main() {

    signal(SIGUSR1, sighandler);
    signal(SIGUSR2, sighandler);
    signal(SIGINT, sighandler);

    printf("Receiver pid: %d\n", getpid());
    printf("Input sender pid: ");
    scanf("%d", &sender_pid);
    kill(sender_pid, SIGUSR2);

    while (1) {
        usleep(30);
    }
}
