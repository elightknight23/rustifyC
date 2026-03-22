#include <stdio.h>

void provably_safe(int *out) {
  int arr[100];
  // Loop bounds explicitly safe
  for (int i = 0; i < 100; i++) {
    arr[i] = i;
  }
  *out = arr[50];
}

void provably_safe_offset(int *out) {
  int arr[100];
  for (int i = 10; i < 50; i++) {
    arr[i - 10] = i;
  }
  *out = arr[20];
}

void needs_check(int *out) {
  int arr[100];
  // Unsafe upper bound
  for (int i = 0; i < 200; i++) {
    arr[i] = i;
  }
  *out = arr[90];
}

int main() {
  int out = 0;
  provably_safe(&out);
  provably_safe_offset(&out);
  needs_check(&out);
  printf("SCEV optimization testing complete. %d\n", out);
  return 0;
}
