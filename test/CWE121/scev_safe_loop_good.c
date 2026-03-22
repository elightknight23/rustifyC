#include <stdio.h>

void safe_scev(int* out) {
    int arr[100];
    for(int i = 0; i < 100; i++) {
        arr[i] = i; // should be provably safe by SCEV
    }
    *out = arr[50];
}

int main() {
    int out = 0;
    safe_scev(&out);
    return 0;
}
