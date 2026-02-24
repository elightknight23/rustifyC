int main() {
    // 'volatile' forces the compiler to emit the store instruction
    // even if it looks like a bug.
    volatile int buf[5];
    buf[10] = 0; // Constant Overflow
    return 0;
}