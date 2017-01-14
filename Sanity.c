//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


#define numOfChild 30
int cidPid[numOfChild];
//
int getCid(int pid){
    for( int i = 0 ; i < numOfChild ; i++ ){
        if( cidPid[i] == pid )
            return i;
    }

    return 0;
}

int main(void) {
    int childPid[numOfChild];


    for (int i = 0; i < numOfChild; i++) {
        childPid[i] = 0;
    }

    for (int i = 0; i < numOfChild; i++) {
        childPid[i] = fork();
        if (childPid[i] < 0) {
            printf(1, "fork failed\n");
            exit();
        } else if (childPid[i] == 0) {
            switch (i % 3) {
                case 0 :
                    nice();
                    break;
                case 1 :
                    niceTwo();
                    break;
            }
            for (long j = 0; j < 10; j++) {
                printf(1, "cid : %d\n", i);
            }
            exit();
        } else if (childPid[i] > 0) {
            cidPid[i] = childPid[i];
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

    for (int i = 0; i < numOfChild ; i++) {
        int wtime;
        int rtime;

        printf(1 , "iiiiiiiiiiiiiiiiiiiiiii %d\n" , i);
        int pid = wait2(&wtime, &rtime);

        int cid = getCid(pid);

        switch (cid%3){
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
        printf(1, "child %d : \n", cid);
        printf(1, "wtime : %d , rtime : %d , turnaround time : %d\n\n", wtime, rtime, wtime + rtime);
    }


    printf(1, "Average waiting time for all children : %f\n", sumOfWtime / numOfChild);
    printf(1, "Average turnaround time for all children : %f\n", (sumOfWtime + sumOfRtime) / numOfChild);
    printf(1, "Average waiting time for queue 1 children : %f\n", sumOfWtimeQ1 / numOfChild);
    printf(1, "Average turnaround time for queue 1 children : %f\n", (sumOfWtimeQ1 + sumOfRtimeQ1) / numOfChild);
    printf(1, "Average waiting time for queue 2 children : %f\n", sumOfWtimeQ2 / numOfChild);
    printf(1, "Average turnaround time for queue 2 children : %f\n", (sumOfWtimeQ2 + sumOfRtimeQ2) / numOfChild);
    printf(1, "Average waiting time for queue 3 children : %f\n", sumOfWtimeQ3 / numOfChild);
    printf(1, "Average turnaround time for queue 3 children : %f\n", (sumOfWtimeQ3 + sumOfRtimeQ3) / numOfChild);

    exit();
}
