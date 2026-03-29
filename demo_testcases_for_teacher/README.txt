RustifyC - Demo Scripts & Test Cases Overview
=============================================
A quick reference guide to explain each script and test file during your presentation.

------------------------------------------------------------
DEMO SCRIPTS (run from the project root: /rustifyC)
------------------------------------------------------------

demo_all.sh
  - Runs ALL phases back to back in one session.
  - Starts with Phase 1 & 2, then chains 3 -> 4 -> 5 -> 6.
  - Best script to use for a full end-to-end walkthrough.
  - Input Files: Inline (written to /tmp/rustifyc_demo/ at runtime by each sub-script)

demo_phase3.sh
  - Shows the SCEV math optimization.
  - Proves a safe loop is safe at compile-time, skipping runtime checks.
  - Runs the performance benchmark after to show the speedup.
  - Input Files: Inline demo written to /tmp/rustifyc_demo/test_scev_demo.c
                 Perf benchmark: test/test_scev_perf.c

demo_phase4.sh
  - Shows uninitialized memory detection.
  - A variable is read before it's ever written.
  - RustifyC catches it at compile-time AND injects a trap at runtime.
  - Input File: Inline demo written to /tmp/rustifyc_demo/test_uninit_demo.c

demo_phase5.sh
  - Shows integer overflow detection.
  - Adds 1 to the maximum integer value (INT_MAX).
  - Normal C silently wraps to a huge negative number; RustifyC aborts.
  - Input File: Inline demo written to /tmp/rustifyc_demo/test_overflow_demo.c

demo_phase6.sh
  - Shows stack escape / dangling pointer detection.
  - Three tests: returning a stack address, storing to global, and a safe case.
  - Only the safe case should compile without warnings.
  - Input Files: test/test_stack_escape_return.c
                 test/test_stack_escape_global.c
                 test/test_stack_safe.c

run_all_benchmarks.sh
  - Compares Native C vs RustifyC (Dynamic) vs RustifyC (SCEV) performance.
  - Prints accuracy results from the NIST Juliet test suite.
  - Compiles the real-world combined file and shows all 6 bugs caught live.
  - Input Files: test/test_scev_perf.c           (SCEV benchmark)
                 test/juliet_results.txt          (accuracy summary)
                 demo_testcases_for_teacher/07_real_world_combined.c  (live compile)


------------------------------------------------------------
TEST CASE FILES (in demo_testcases_for_teacher/)
------------------------------------------------------------

01_spatial_safety_static.c
  - Buffer of size 5, writing to index 10.
  - Bug is obvious at compile-time. Phase 1 blocks it immediately.

02_spatial_safety_dynamic.c
  - Buffer of size 10, index comes from user input (argv).
  - Compiler can't know value ahead of time; Phase 2 injects a runtime check.

03_scev_loop_optimization.c
  - Safe loop: index goes from 0 to 99, array size is 100.
  - Phase 3 proves this mathematically and removes any injected check (0% overhead).

04_uninitialized_memory.c
  - A variable is declared but never given a starting value before being read.
  - Phase 4 traces the Def-Use chain and injects a panic trap.

05_integer_overflow.c
  - Adds 1 to INT_MAX, which silently wraps to a large negative in standard C.
  - Phase 5 uses hardware CPU overflow flags to catch this and abort safely.

06_stack_escape.c
  - Returns the address of a local variable from a function.
  - That memory is destroyed when the function ends -- a dangling pointer.
  - Phase 6 detects the escape and blocks compilation.

07_real_world_combined.c  <-- THE BIG ONE
------------------------------------------------------------
HOW TO RUN ON YOUR OWN CUSTOM FILE
------------------------------------------------------------
To secure your own C code, use the following `clang` command:

  clang -O1 -fpass-plugin=./build/rustifyC.so your_code.c -o secure_binary
  clang -O1 -fpass-plugin=./build/rustifyC.so demo_testcases_for_teacher/01_spatial_safety_static.c -o secure_binary
  clang -O1 -fpass-plugin=./build/rustifyC.so \
  demo_testcases_for_teacher/02_spatial_safety_dynamic.c \
  -o secure_dynamic -mllvm -stats

Explanation of flags:
-  -O1: Required for LLVM's math proofs (Phase 3) to run and optimize out safe checks.
-  -fpass-plugin=./build/rustifyC.so: This is the actual "RustifyC" engine we built.
-  -mllvm -stats: (Optional) Add this to the end to see how many checks were added or elided!
-  -g: (Optional) Add this to get better error messages with line numbers.


# Compile the dangerous file (Use -O0 so the optimizer doesn't delete the bug!)
clang -O0 -fpass-plugin=./build/rustifyC.so demo_testcases_for_teacher/02_spatial_safety_dynamic.c -o secure_dynamic
clang -O0 -fpass-plugin=./build/rustifyC.so demo_testcases_for_teacher/07_real_world_combined.c -o secure_dynamic

# Run it with index 5 (Safe)
./secure_dynamic 5

# Run it with index 99 (Dangerous! It will trap/abort safely)
./secure_dynamic 99
echo "Exit Code: $?"  # This will show 101 or 134, proving RustifyC caught it!
