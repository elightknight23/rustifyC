// test_exploit.c
// Demonstrates the intra-frame corruption attack

#include <stdio.h>
#include <stdlib.h>

struct vulnerable {
  int buffer[4];
  int admin_flag;
} data;

void check_access(int user_index) {
  data.admin_flag = 0; // Reset to non-admin

  printf("[*] User attempting to write at index %d...\n", user_index);

  // If user_index = 4, this writes directly into admin_flag
  data.buffer[user_index] = 1;

  if (data.admin_flag) {
    printf("CRITICAL: admin access GRANTED (flag = %d)\n", data.admin_flag);
    printf("Privilege escalation successful!\n");
  } else {
    printf("Access to admin denied (flag = %d)\n", data.admin_flag);
    printf("Success!");
  }
}

int main(int argc, char *argv[]) {
  int attack_index = argc > 1 ? atoi(argv[1]) : 3;

  printf("=== Testing Data-Oriented Attack ===\n\n");
  check_access(attack_index);

  return 0;
}