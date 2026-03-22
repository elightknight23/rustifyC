#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[411];
    int idx = argc + 411; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}