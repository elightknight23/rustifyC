#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[75];
    for(int i = 0; i < 75; i++) { // safe
        arr[i] = i; 
    }
    return 0;
}