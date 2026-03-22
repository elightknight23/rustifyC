#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[208];
    int idx = argc + 208; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}