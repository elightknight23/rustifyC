#include <stdio.h>
#include <stdlib.h>

void exploit_me(int user_index) {
  int buffer[5];
  printf("Writing to buffer[%d]...\n", user_index);
  // DYNAMIC OVERFLOW: The index depends on runtime input
  buffer[user_index] = 0xDEADBEEF;
  printf("Write successful.\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s <index>\n", argv[0]);
    return 1;
  }

  int index = atoi(argv[1]);
  exploit_me(index);

  return 0;
}
