#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[58];
    int idx = argc + 58; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}