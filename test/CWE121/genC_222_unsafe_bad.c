#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[260];
    int idx = argc + 260; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}