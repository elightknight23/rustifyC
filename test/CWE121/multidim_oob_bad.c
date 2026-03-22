#include <stdio.h>

int main() {
    int matrix[5][5];
    matrix[1][6] = 42; // static multi-dimensional violation
    return 0;
}
