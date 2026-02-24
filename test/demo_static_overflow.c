#include <stdio.h>

void exploit_me() {
  int buffer[5];
  // STATIC OVERFLOW: Index 5 is out of bounds for an array of size 5
  buffer[5] = 0xDEADBEEF;
}

int main() {
  printf("Running static overflow test...\n");
  exploit_me();
  return 0;
}
