//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


#define NUM_OF_CHILD 5

int main(void) {
    int childPid[NUM_OF_CHILD];
    sem_init(0 , 1);

    for (int i = 0; i < NUM_OF_CHILD; i++) {
        childPid[i] = 0;
    }

    for (int i = 0; i < NUM_OF_CHILD; i++) {
        childPid[i] = fork();
        if (childPid[i] < 0) {
//            wait_semaphore();
            printf(1, "fork failed\n");
//            signal_semaphore();
            exit();
        } else if (childPid[i] == 0) {
            set_cid(i);
            if( i%3 == 0 ){
                nice();
            }else if ( i%3 == 1 ){
                nice();
//                nice();
            }else{
            }

            for (long j = 0; j < 1000; j++) {
//                wait_semaphore();
                sem_wait(0,1);
                printf(1, "cid : %d\n", i);
                sem_signal(0,1);
//                signal_semaphore();
            }
            exit();
        } else if (childPid[i] > 0) {
            continue;
        }
    }


    int sumOfWtime = 0;
    int sumOfRtime = 0;
    int sumOfRtimeQ1 = 0;
    int sumOfRtimeQ2 = 0;
    int sumOfRtimeQ0 = 0;
    int sumOfWtimeQ1 = 0;
    int sumOfWtimeQ2 = 0;
    int numOfQ1 = 0;
    int numOfQ2 = 0;
    int numOfQ0 = 0;
    int sumOfWtimeQ0 = 0;

    int info[NUM_OF_CHILD][3] ;
    for (int i = 0; i < NUM_OF_CHILD; i++) {
        int wtime;
        int rtime;

        int cid = wait2(&wtime, &rtime);
//        pids[i] = pid;

        switch (cid % 3) {
            case 0 :
                sumOfRtimeQ0 += rtime;
                sumOfWtimeQ0 += wtime;
                numOfQ0++;
                break;
            case 1 :
                sumOfRtimeQ0 += rtime;
                sumOfWtimeQ0 += wtime;
                numOfQ0++;
                break;
            case 2 :
                sumOfRtimeQ2 += rtime;
                sumOfWtimeQ2 += wtime;
                numOfQ2++;
                break;
        }

        sumOfRtime += rtime;
        sumOfWtime += wtime;
//        wait_semaphore();
        info[cid][0] = wtime;
        info[cid][1] = rtime;
        info[cid][2] = wtime + rtime;
//        signal_semaphore();
    }

    for( int i = 0 ; i < NUM_OF_CHILD ; i++ ){
       // int cid = getCid(pids[i]);

        sem_wait(0,1);
        printf(1, "child %d : \n", i);
        printf(1, "wtime : %d , rtime : %d , turnaround time : %d\n\n", info[i][0], info[i][1], info[i][2]);
        sem_signal(0,1);
    }

    //double average1 = (double) sumOfWtime /  numOfChild ;
//    wait_semaphore();
    sem_wait(0,1);
    printf(1, "Average waiting time for all children : %d \n", sumOfWtime / NUM_OF_CHILD);
    printf(1, "Average turnaround time for all children : %d \n",
           (sumOfWtime + sumOfRtime) / NUM_OF_CHILD);
    printf(1, "Average waiting time for queue 0 children : %d \n",  sumOfWtimeQ0 /  numOfQ0);
    printf(1, "Average turnaround time for queue 0 children : %d \n",
           (sumOfWtimeQ0 + sumOfRtimeQ0) / numOfQ0);
    printf(1, "Average waiting time for queue 1 children : %d \n", sumOfWtimeQ1 /  numOfQ1);
    printf(1, "Average turnaround time for queue 1 children : %d \n",
           (sumOfWtimeQ1 + sumOfRtimeQ1) / numOfQ1);
    printf(1, "Average waiting time for queue 2 children : %d \n",  sumOfWtimeQ2 /  numOfQ2);
    printf(1, "Average turnaround time for queue 2 children : %d \n",
           (sumOfWtimeQ2 + sumOfRtimeQ2) / numOfQ2);
//    signal_semaphore();

    sem_signal(0,1);

    sem_destroy(0);
    exit();
}