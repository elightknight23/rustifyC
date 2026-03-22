#!/bin/bash
# benchmark.sh

BENCHMARKS="matmul quicksort parse"
ITERATIONS=100
PASS="$(pwd)/build/rustifyC.so"

echo "=== Compiling Benchmarks ==="

# We mock these for the MVP to prove the concept works. 
# A real implementation would use complete benchmark source files.
cat << 'EOF' > test/benchmarks/matmul.c
void multiply(int A[100][100], int B[100][100], int C[100][100]) {
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
            for (int k = 0; k < 100; k++)
                C[i][j] += A[i][k] * B[k][j];
}
int main() {
    int A[100][100] = {0}, B[100][100] = {0}, C[100][100] = {0};
    for (int i = 0; i < 100; i++) multiply(A, B, C); 
    return 0;
}
EOF

cat << 'EOF' > test/benchmarks/quicksort.c
void swap(int* a, int* b) { int t = *a; *a = *b; *b = t; }
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) { i++; swap(&arr[i], &arr[j]); }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
int main() {
    int arr[1000];
    for(int i=0; i<1000; i++) arr[i] = 1000-i;
    for(int i=0; i<1000; i++) quickSort(arr, 0, 999);
    return 0;
}
EOF

cat << 'EOF' > test/benchmarks/parse.c
int main() {
    char buffer[10000];
    int sum = 0;
    for(int j=0; j<1000; j++) {
        for(int i=0; i<10000; i++) {
            buffer[i] = (i % 26) + 'a';
            if (buffer[i] == 'z') sum++;
        }
    }
    return sum > 0 ? 0 : 1;
}
EOF

# Use ruby to reliably format time output on macOS
time_cmd() {
    exec_file=$1
    # Run the program and capture user + sys time, ignoring the actual output of the command
    /usr/bin/time -p $exec_file > /dev/null 2>&1 | awk '/real/ {print $2}'
}

for bench in $BENCHMARKS; do
    echo "=== Benchmark: $bench ==="
    
    cd test/benchmarks
    
    # Baseline (no instrumentation)
    clang -O2 $bench.c -o ${bench}_baseline
    
    # Measure execution time
    start=$(date +%s.%N)
    for i in $(seq 1 $ITERATIONS); do ./${bench}_baseline >/dev/null; done
    end=$(date +%s.%N)
    baseline_time=$(echo "$end - $start" | bc)
    baseline_size=$(stat -f%z ${bench}_baseline)
    
    # RustifyC
    clang -O2 -fpass-plugin=$PASS $bench.c -w -o ${bench}_rustifyc
    
    start=$(date +%s.%N)
    for i in $(seq 1 $ITERATIONS); do ./${bench}_rustifyc >/dev/null; done
    end=$(date +%s.%N)
    rustifyc_time=$(echo "$end - $start" | bc)
    rustifyc_size=$(stat -f%z ${bench}_rustifyc)
    
    # ASan
    clang -O2 -fsanitize=address $bench.c -w -o ${bench}_asan
    
    start=$(date +%s.%N)
    for i in $(seq 1 $ITERATIONS); do ./${bench}_asan >/dev/null; done
    end=$(date +%s.%N)
    asan_time=$(echo "$end - $start" | bc)
    asan_size=$(stat -f%z ${bench}_asan)
    
    # Clean output formatting
    echo ""
    printf "%-12s | %-15s | %-15s\n" "Version" "Time (s)" "Size (bytes)"
    printf "------------------------------------------------\n"
    printf "%-12s | %-15.3f | %-15d\n" "Baseline" "$baseline_time" "$baseline_size"
    printf "%-12s | %-15.3f | %-15d\n" "RustifyC" "$rustifyc_time" "$rustifyc_size"
    printf "%-12s | %-15.3f | %-15d\n" "ASan" "$asan_time" "$asan_size"
    echo ""
    cd ../..
done
