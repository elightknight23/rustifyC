#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[192];
    int idx = argc + 192; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}