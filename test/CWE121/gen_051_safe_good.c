#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main(int argc, char **argv) {
    int arr[74];
    for(int i = 0; i < 74; i++) { // safe
        arr[i] = i; 
    }
    return 0;
}