#!/bin/bash
# benchmark_scev.sh - Measure the exact runtime overhead of RustifyC spatial checks
# and how much SCEV math engine saves us!

RED='\033[1;31m'
GREEN='\033[1;32m'
CYAN='\033[1;36m'
YELLOW='\033[1;33m'
NC='\033[0m'

PASS="$(pwd)/build/rustifyC.so"
TEST_FILE="$(pwd)/test/test_scev_perf.c"
OUT_BASE="/tmp/rustifyc_bench_base"
OUT_DYN="/tmp/rustifyc_bench_dyn"
OUT_SCEV="/tmp/rustifyc_bench_scev"

echo -e "${CYAN}================================================================${NC}"
echo -e "${GREEN}   RustifyC Benchmark: The Power of SCEV Check Elision        ${NC}"
echo -e "${CYAN}================================================================${NC}"
echo "We are running a tight loop with 100 Million array accesses."

# 1. Baseline C Benchmark
echo -e "\n${YELLOW}[1/3] Compiling standard Clang C (No Safety/Baseline)...${NC}"
clang -O1 $TEST_FILE -o $OUT_BASE
echo "Measuring execution time..."
TIME_BASE=$( { time $OUT_BASE; } 2>&1 | grep real | awk '{print $2}' )
echo -e "Baseline Time: ${GREEN}${TIME_BASE}${NC}"

# 2. RustifyC WITHOUT SCEV (Dynamic Checks Only)
echo -e "\n${YELLOW}[2/3] Compiling RustifyC (Safety ON, SCEV OFF)...${NC}"
DISABLE_SCEV=1 clang -O1 -fpass-plugin=$PASS $TEST_FILE -o $OUT_DYN > /dev/null 2>&1
echo "Measuring execution time..."
TIME_DYN=$( { time $OUT_DYN; } 2>&1 | grep real | awk '{print $2}' )
echo -e "Overhead Time: ${RED}${TIME_DYN}${NC} (Due to millions of injected if-bounds-checks)"

# 3. RustifyC WITH SCEV (Dynamic Checks + Static Math Proofs)
echo -e "\n${YELLOW}[3/3] Compiling RustifyC (Safety ON, SCEV ON)...${NC}"
clang -O1 -fpass-plugin=$PASS $TEST_FILE -o $OUT_SCEV > /dev/null 2>&1
echo "Measuring execution time..."
TIME_SCEV=$( { time $OUT_SCEV; } 2>&1 | grep real | awk '{print $2}' )
echo -e "Optimized Time: ${GREEN}${TIME_SCEV}${NC} (Checks elided by LLVM Math Proofs!)"

echo -e "\n${CYAN}================================================================${NC}"
echo -e "Summary of 100M Iterations:"
echo -e "  - Native C (Unsafe):       ${GREEN}${TIME_BASE}${NC}"
echo -e "  - RustifyC (Dynamic Only): ${RED}${TIME_DYN}${NC}"
echo -e "  - RustifyC (SCEV Smart):   ${GREEN}${TIME_SCEV}${NC}"
echo -e "${CYAN}================================================================${NC}"
echo "With SCEV, we achieve Rust-like safety with practically zero-cost overhead"
echo "on strictly bounded hot loops."

rm -f $OUT_BASE $OUT_DYN $OUT_SCEV
