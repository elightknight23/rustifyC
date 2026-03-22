#!/bin/bash
# test/compare_clang.sh

PASS="$(pwd)/build/rustifyC.so"
TEST_DIR="test/CWE121"

echo "=== RustifyC vs Apple Clang Competitive Analysis ==="
echo "Testing compilation of UNSAFE files (Vulnerabilities)"

CLANG_CAUGHT=0
RUSTIFYC_CAUGHT=0
TOTAL_BAD=0

# Clean old binaries
rm -f /tmp/clang_out /tmp/rust_out

for testfile in $TEST_DIR/*_bad.c $TEST_DIR/demo_*.c $TEST_DIR/dynamic_*_bad.c; do
    if [ ! -f "$testfile" ]; then continue; fi
    TOTAL_BAD=$((TOTAL_BAD + 1))
    basename=$(basename $testfile)
    echo "Evaluating $basename..."
    
    # Apple Clang Check (Static Warnings)
    # We compile with -O2 -Wall -Wextra. If it exits with 0 and has no warnings about the bug, it missed it.
    clang -O2 -Wall -Wextra -Wno-unused-variable -Wno-unused-value "$testfile" -c -o /dev/null > /tmp/clang_out 2>&1
    clang_warns=$(grep -i -E "warning:|error:" /tmp/clang_out | wc -l)
    
    # RustifyC Check (Compiler Pass)
    clang -O0 -Xclang -disable-O0-optnone -Wno-everything -fpass-plugin=$PASS "$testfile" -c -o /dev/null > /tmp/rust_out 2>&1
    rustify_warns=$(grep -i -E "error\[CWE-121\]|error\[E0381\]|error\[E0597\]" /tmp/rust_out | wc -l)
    
    # Dynamic Check fallback for RustifyC (if static didn't catch it, does it inject a panic?)
    # We compile it to an executable and check if it has the panic block.
    # Note: To be precise, our pass adds `rustifyc_panic` function. We can just check the LLVM IR.
    clang -O0 -Xclang -disable-O0-optnone -Wno-everything -fpass-plugin=$PASS "$testfile" -S -emit-llvm -o /tmp/rust_ir.ll 2>/dev/null
    rustify_dynamic=$(grep -c "\.rustifyc\." /tmp/rust_ir.ll)
    
    clang_caught_this=0
    rustify_caught_this=0
    
    if [ $clang_warns -gt 0 ]; then
        CLANG_CAUGHT=$((CLANG_CAUGHT + 1))
        clang_caught_this=1
    fi
    
    if [ $rustify_warns -gt 0 ] || [ $rustify_dynamic -gt 0 ]; then
        RUSTIFYC_CAUGHT=$((RUSTIFYC_CAUGHT + 1))
        rustify_caught_this=1
    fi
    
    # Print cases where RustifyC beat Clang
    if [ $clang_caught_this -eq 0 ] && [ $rustify_caught_this -eq 1 ]; then
        echo "🏆 RustifyC Superiority: Clang missed vulnerability in $basename, but RustifyC secured it!"
    fi
done

echo "------------------------------------------------------"
echo "Total Vulnerable Test Cases Evaluated: $TOTAL_BAD"
echo "Apple Clang (-Wall -Wextra) Detected:  $CLANG_CAUGHT / $TOTAL_BAD"
echo "RustifyC Detected (Static/Dynamic):    $RUSTIFYC_CAUGHT / $TOTAL_BAD"
echo ""

CLANG_PERCENT=$(echo "scale=2; ($CLANG_CAUGHT / $TOTAL_BAD) * 100" | bc)
RUSTIFY_PERCENT=$(echo "scale=2; ($RUSTIFYC_CAUGHT / $TOTAL_BAD) * 100" | bc)

echo "Apple Clang Recall: ${CLANG_PERCENT}%"
echo "RustifyC Recall:    ${RUSTIFY_PERCENT}%"
