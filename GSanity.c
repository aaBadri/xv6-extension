//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


int main(void) {
    int childPid = 0 ;

    printf(1, "Father pid is %d\n" , getpid());

    sleep(200);

    childPid = fork();
    if (childPid < 0) {
        printf(1, "fork failed\n");
        exit();
    } else if (childPid == 0) {
        int pid = getpid();
        for (int i = 0; i < 50; i++) {
            printf(1, "process %d is printing for the %d time\n", pid, i);
        }
        exit();
    } else if (childPid > 0) {
        //
    }


    wait();
    int pid = getpid();
    for (int i = 0; i < 50; i++) {
        printf(1, "process %d is printing for the %d time\n", pid, i);
    }
    exit();
}
