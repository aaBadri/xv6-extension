//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


#define numOfChild 5
int cidPid[numOfChild];

//
int getCid(int pid) {
    for (int i = 0; i < numOfChild; i++) {
        if (cidPid[i] == pid)
            return i;
    }

    return 0;
}

int main(void) {
    int childPid[numOfChild];
    sem_init(0 , 1);

    for (int i = 0; i < numOfChild; i++) {
        childPid[i] = 0;
    }

    for (int i = 0; i < numOfChild; i++) {
        childPid[i] = fork();
        if (childPid[i] < 0) {
//            wait_semaphore();
            printf(1, "fork failed\n");
//            signal_semaphore();
            exit();
        } else if (childPid[i] == 0) {
            cidPid[i] = getpid();
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
    int sumOfRtimeQ3 = 0;
    int sumOfWtimeQ1 = 0;
    int sumOfWtimeQ2 = 0;
    int sumOfWtimeQ3 = 0;

    for (int i = 0; i < numOfChild; i++) {
        int wtime;
        int rtime;

        int pid = wait2(&wtime, &rtime);

        int cid = getCid(pid);

        switch (cid % 3) {
            case 0 :
                sumOfRtimeQ1 += rtime;
                sumOfWtimeQ1 += wtime;
                break;
            case 1 :
                sumOfRtimeQ2 += rtime;
                sumOfWtimeQ2 += wtime;
                break;
            case 2 :
                sumOfRtimeQ3 += rtime;
                sumOfWtimeQ3 += wtime;
                break;
        }

        sumOfRtime += rtime;
        sumOfWtime += wtime;
//        wait_semaphore();
        sem_wait(0,1);
        printf(1, "child %d : \n", cid);
        printf(1, "wtime : %d , rtime : %d , turnaround time : %d\n\n", wtime, rtime, wtime + rtime);
        sem_signal(0,1);
//        signal_semaphore();
    }


    //double average1 = (double) sumOfWtime /  numOfChild ;
//    wait_semaphore();
    sem_wait(0,1);
    printf(1, "Average waiting time for all children : %d \n", sumOfWtime / numOfChild);
    printf(1, "Average turnaround time for all children : %d \n",
           (sumOfWtime + sumOfRtime) / numOfChild);
    printf(1, "Average waiting time for queue 1 children : %d \n",  sumOfWtimeQ1 /  numOfChild);
    printf(1, "Average turnaround time for queue 1 children : %d \n",
           (sumOfWtimeQ1 + sumOfRtimeQ1) / numOfChild);
    printf(1, "Average waiting time for queue 2 children : %d \n", sumOfWtimeQ2 /  numOfChild);
    printf(1, "Average turnaround time for queue 2 children : %d \n",
           (sumOfWtimeQ2 + sumOfRtimeQ2) / numOfChild);
//    printf(1, "Average waiting time for queue 3 children : %d \n",  sumOfWtimeQ3 /  numOfChild);
//    printf(1, "Average turnaround time for queue 3 children : %d \n",
//           (sumOfWtimeQ3 + sumOfRtimeQ3) / numOfChild);
//    signal_semaphore();

    sem_signal(0,1);

    sem_destroy(0);
    exit();
}
