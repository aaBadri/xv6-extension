//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


int main(void) {
    int childPid[10];
    for (int i = 0; i < 10; i++) {
        childPid[i] = 0;
    }

    for (int i = 0; i < 10; i++) {
        childPid[i] = fork();
        if (childPid[i] < 0) {
            printf(1, "fork failed\n");
            exit();
        } else if (childPid[i] == 0) {
            int pid = getpid();
            for (int i = 0; i < 3; i++) {
                printf(1, "Child %d prints for the %d time\n", pid, i);
            }
            exit();
        } else if (childPid[i] > 0) {
            continue;
        }
    }


    for (int i = 0; i < 10; i++) {
        int wtime;
        int rtime;

        wait2(&wtime, &rtime);

        printf(1, "child %d : \n", i);
        printf(1, "wtime : %d , rtime : %d , turnaround time : %d\n\n", wtime, rtime, wtime + rtime);
    }
    exit();
}
