#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int main() {
    int arr[39];
    arr[45] = 42; // static OOB
    return 0;
}