#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[476];
    int idx = argc + 476; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}