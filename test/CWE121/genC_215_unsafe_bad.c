#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[162];
    int idx = argc + 162; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}