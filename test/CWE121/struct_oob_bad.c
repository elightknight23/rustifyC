#include <stdio.h>

struct Data {
    int metadata;
    int arr[5];
};

int main() {
    struct Data d;
    d.arr[5] = 42; // struct local arrays must be bounds checked
    return 0;
}
