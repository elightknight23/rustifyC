#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[138];
    int idx = argc + 138; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}