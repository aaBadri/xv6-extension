//
// Created by mma on 1/12/17.
//
#include "types.h"
#include "user.h"


#define numOfChild 10

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
            for (int i = 0; i < 1000; i++) {

            }
            exit();
        } else if (childPid[i] > 0) {
            continue;
        }
    }


    for (int i = 0; i < numOfChild; i++) {
        wait();
    }
    exit();
}
