#include <stdio.h>

int main() {
    int arr[10];
    for (int i = 0; i <= 10; ++i) { // classic <= off-by-one loop boundary
        arr[i] = i;
    }
    return 0;
}
