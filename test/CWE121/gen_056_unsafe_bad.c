#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[16];
    for(int i = 0; i <= 16; i++) { // off by one
        arr[i] = i; 
    }
    return 0;
}