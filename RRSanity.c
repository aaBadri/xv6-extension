//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"

#define NUM_OF_CHILD 10
int main(void) {
    int childPid[NUM_OF_CHILD];
    sem_init(0 , 1);

    for (int i = 0; i < NUM_OF_CHILD; i++) {
        childPid[i] = 0;
    }

    for (int i = 0; i < NUM_OF_CHILD; i++) {
        childPid[i] = fork();
        if (childPid[i] < 0) {
            printf(1, "fork failed\n");
            exit();
        } else if (childPid[i] == 0) {
            int pid = getpid();
            for (int j = 0; j < 1000; j++) {
                sem_wait(0,1);
                printf(1, "Child %d prints for the %d time\n", pid, j);
                sem_signal(0,1);
            }
            exit();
        } else if (childPid[i] > 0) {
            continue;
        }
    }


    int info[NUM_OF_CHILD][3] ;
    for (int i = 0; i < NUM_OF_CHILD; i++) {
        int wtime;
        int rtime;

        wait2(&wtime, &rtime);

        info[i][0] = wtime;
        info[i][1] = rtime;
        info[i][2] = wtime + rtime;
    }

    for( int i = 0 ; i < NUM_OF_CHILD ; i++ ){
        sem_wait(0,1);
        printf(1, "child %d : \n", i);
        printf(1, "wtime : %d , rtime : %d , turnaround time : %d\n\n", info[i][0], info[i][1], info[i][2]);
        sem_signal(0,1);
    }

    sem_destroy(0);
    exit();
}
