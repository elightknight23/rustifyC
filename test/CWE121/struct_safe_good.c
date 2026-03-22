#include <stdio.h>

struct Data {
    int metadata;
    int arr[5];
};

int main() {
    struct Data d;
    d.arr[4] = 42; // maximum bound of struct-contained array
    return 0;
}
