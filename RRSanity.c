//
// Created by mma on 1/12/17.
//

#include <sched.h>
#include <stdio.h>
#include "user.h"

int main(void){
    int childPid[10];
    for( int i = 0 ; i < 10 ; i++ ){
        childPid[i] = 0;
    }

    for( int i = 0 ; i < 10 ; i++ ){
        childPid[i] = fork();
        if (childPid[i] < 0) {
            perror("fork failed");
            exit();
        } else if (childPid[i] == 0) {
          //  TODO
            exit();
        } else if (childPid[i] > 0) {
            continue;
        }
    }

    for( int i = 0 ; i < 10 ; i++ ){
        wait();
    }
}
