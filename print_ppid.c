#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
    printf(1,"The parent of current process is %d\n", getppid());
    exit();
}