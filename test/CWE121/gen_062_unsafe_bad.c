#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[48];
    for(int i = 0; i <= 48; i++) { // off by one
        arr[i] = i; 
    }
    return 0;
}