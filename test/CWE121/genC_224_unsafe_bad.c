#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[477];
    int idx = argc + 477; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}