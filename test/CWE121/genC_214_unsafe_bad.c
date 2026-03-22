#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[182];
    int idx = argc + 182; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}