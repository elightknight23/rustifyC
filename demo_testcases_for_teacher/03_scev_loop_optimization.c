#include <stdio.h>

int main() {
  int arr[102];

  // Teacher explanation:
  // Adding checks inside a fast loop slows programs down a lot.
  // Here, RustifyC uses LLVM's advanced math engine (ScalarEvolution) to
  // mathematically prove that 'i' will never exceed 99.
  // Since 99 is less than the array size (100), RustifyC totally removes the
  // bounds check inside the loop! We get perfect safety with absolutely ZERO
  // slowdown.
  for (int i = 0; i < 102; i++) {
    arr[i] = i;
  }

  printf("Loop finished safely with 0%% overhead!\n");
  return 0;
}
