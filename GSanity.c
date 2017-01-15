//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


int main(void) {
    int childPid = 0 ;

    sem_init(0,1);
    sem_wait(0,1);
    printf(1, "Father pid is %d\n" , getpid());
    sem_signal(0,1);

    sleep(200);

    childPid = fork();
    if (childPid < 0) {
        printf(1, "fork failed\n");
        exit();
    } else if (childPid == 0) {
        int pid = getpid();
        for (int i = 0; i < 50; i++) {
            sem_wait(0,1);
            printf(1, "process %d is printing for the %d time\n", pid, i);
            sem_signal(0,1);
        }
        exit();
    } else if (childPid > 0) {
        //
    }



    wait();
    int pid = getpid();
    for (int i = 0; i < 50; i++) {
        sem_wait(0,1);
        printf(1, "process %d is printing for the %d time\n", pid, i);
        sem_signal(0,1);
    }
    sem_destroy(0);
    exit();
}
