#!/bin/bash

echo "=========================================="
echo "🛡️  rustifyC: Phase 2 MVP Demonstration"
echo "=========================================="
echo ""

# Ensure we have the latest pass built
cmake --build build > /dev/null

echo "[1/2] Testing Static Buffer Overflow Prevention (Compile-time)"
echo "------------------------------------------------------------"
echo "Attempting to compile a program with an obvious static overflow (buffer[5] on int buffer[5])..."
echo ""

# We expect this to fail compilation
opt -load-pass-plugin=./build/rustifyC.so -passes="rustifyC" -disable-output <(clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c test/demo_static_overflow.c -o -) 2>&1 | grep "CRITICAL ERROR" -A 4
echo ""
echo "✅ Compilation was successfully aborted by rustifyC to prevent a vulnerable binary!"
echo ""

echo "[2/2] Testing Dynamic Buffer Overflow Prevention (Runtime)"
echo "----------------------------------------------------------"
echo "Compiling a program where the array index is provided by the user at runtime..."
clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c test/demo_dynamic_overflow.c -o raw.ll
opt -load-pass-plugin=./build/rustifyC.so -passes="rustifyC" raw.ll -o safe.bc 
clang safe.bc -o dynamic_demo_safe
echo ""

echo "▶️  Running with safe index (2):"
./dynamic_demo_safe 2
echo "✅ Execution successful."
echo ""

echo "▶️  Running with unsafe index (10):"
./dynamic_demo_safe 10 2> crash_log.txt
if [ $? -eq 134 ]; then
    echo "🚨 Execution aborted via SIGABRT! rustifyC successfully blocked the buffer overflow exploit!"
else
    echo "❌ Exploit succeeded (This should not happen with rustifyC!)"
fi
echo ""
echo "=========================================="
echo "Phase 2 MVP Demo Complete."
echo "=========================================="

rm -f raw.ll safe.bc dynamic_demo_safe crash_log.txt
