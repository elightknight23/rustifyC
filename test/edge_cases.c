#include <stdint.h>
#include <stdio.h>

// 1. Mixed integer widths
void test_int_widths() {
  int arr[10];

  int8_t tiny_idx = -1;
  int16_t small_idx = 5;
  int32_t normal_idx = 7;
  int64_t large_idx = 15;

  // Tiny idx = -1 will be caught by UGE comparison or static analysis
  arr[tiny_idx] = 0;
  arr[small_idx] = 0;  // Safe
  arr[normal_idx] = 0; // Safe

  // Dynamic overflow on large index (assuming 10 bounds)
  arr[large_idx] = 0;
}

// 2. Zero-length arrays (GCC extension)
void test_zero_length() {
  int arr[0];
  arr[0] = 0; // Technically UB, should catch
}

// 3. Array in struct
struct Container {
  int metadata;
  int data[10];
};

void test_struct_array() {
  struct Container c;
  c.data[5] = 0;  // Safe
  c.data[15] = 0; // Should catch
}

// 4. Volatile arrays
void test_volatile() {
  volatile int buf[5];
  buf[10] = 0; // Must catch even with volatile
}

// 5. Unsigned index overflow
void test_unsigned() {
  int arr[10];
  unsigned int idx = 15;
  arr[idx] = 0; // Should catch
}

// 6. Multi-dimensional array
void test_multidim(int dim1, int dim2) {
  int matrix[5][10];
  matrix[dim1][dim2] = 42; // Should inject checks
}

int main(int argc, char **argv) {
  printf("Compiling edge cases...\n");
  return 0;
}
