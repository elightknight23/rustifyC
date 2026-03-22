#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[76];
    for(int i = 0; i <= 76; i++) { // off by one
        arr[i] = i; 
    }
    return 0;
}