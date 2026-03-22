#include <stdio.h>

int main() {
    int arr[5];
    arr[999999] = 1; // major bounds violation
    return 0;
}
