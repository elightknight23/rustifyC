#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[415];
    int idx = argc + 415; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}