#include <stdio.h>
#include <stdlib.h>

#define ITERATIONS 100000000 // 100 million iterations

void hot_loop(int *arr, int size) {
  long long sum = 0;
  // This loop is bounded tightly.
  for (int i = 0; i < 1000; i++) {
    arr[i] = i * 2;
    sum += arr[i];
  }
}

int main() {
  int arr[1000];

  // Call the tight loop many times to exaggerate the overhead differences
  for (int j = 0; j < ITERATIONS / 1000; j++) {
    hot_loop(arr, 1000);
  }

  return 0;
}
