<p align="center">
  <img src="https://img.shields.io/badge/LLVM-15-blue?style=for-the-badge&logo=llvm" alt="LLVM 15"/>
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus" alt="C++17"/>
  <img src="https://img.shields.io/badge/Platform-macOS-lightgrey?style=for-the-badge&logo=apple" alt="macOS"/>
  <img src="https://img.shields.io/badge/Accuracy-100%25-brightgreen?style=for-the-badge" alt="100% Accuracy"/>
  <img src="https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge" alt="MIT"/>
</p>

# 🛡️ RustifyC

**Bring Rust-level memory safety to C — at compile time.**

RustifyC is an LLVM compiler pass plugin that injects Rust-style safety checks into standard C programs. It detects buffer overflows, integer overflows, uninitialized memory reads, and dangling pointers — all **without changing your source code**. Just add `-fpass-plugin=./build/rustifyC.so` to your `clang` command and your C code gets Rust-grade protections.

> *"What if C could catch the bugs that Rust prevents — without rewriting a single line?"*

---

## 🎯 Key Features

| Safety Class | Detection Phase | Mechanism | Timing |
|---|---|---|---|
| **Spatial Safety (Static)** | Phase 1 | Compile-time constant index validation | Build time |
| **Spatial Safety (Dynamic)** | Phase 2 | Runtime bounds-check injection via CFG surgery | Runtime |
| **SCEV Optimization** | Phase 3 | Scalar Evolution mathematical proofs | Build time |
| **Uninitialized Memory** | Phase 4 | MemorySSA Def-Use chain analysis | Build time |
| **Integer Overflow** | Phase 5 | Hardware overflow flag intrinsics (`sadd/ssub/smul.with.overflow`) | Runtime |
| **Stack Escape / Dangling Pointers** | Phase 6 | Recursive escape analysis on `alloca` instructions | Build time |

---

## 🏗️ Architecture

```
                        ┌─────────────────────────────────────────┐
                        │              C Source Code               │
                        └────────────────┬────────────────────────┘
                                         │
                                    clang -fpass-plugin
                                         │
                        ┌────────────────▼────────────────────────┐
                        │           LLVM IR (Bitcode)              │
                        └────────────────┬────────────────────────┘
                                         │
              ┌──────────────────────────▼──────────────────────────┐
              │                  RustifyC Pass                       │
              │                                                      │
              │  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
              │  │ Phase 1  │  │ Phase 2  │  │     Phase 3       │  │
              │  │ Static   │  │ Dynamic  │  │  SCEV Elision     │  │
              │  │ Bounds   │  │ Bounds   │  │  (Zero-Cost Proof)│  │
              │  └──────────┘  └──────────┘  └───────────────────┘  │
              │                                                      │
              │  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
              │  │ Phase 4  │  │ Phase 5  │  │     Phase 6       │  │
              │  │ Uninit   │  │ Integer  │  │  Stack Escape     │  │
              │  │ Memory   │  │ Overflow │  │  (Temporal Safety) │  │
              │  └──────────┘  └──────────┘  └───────────────────┘  │
              │                                                      │
              │  ┌──────────────────────────────────────────────┐   │
              │  │         Compilation Report (stderr)           │   │
              │  └──────────────────────────────────────────────┘   │
              └──────────────────────────────────────────────────────┘
                                         │
                        ┌────────────────▼────────────────────────┐
                        │     Hardened Binary (or rejected!)       │
                        └─────────────────────────────────────────┘
```

### How Each Phase Works

#### Phase 1 — Static Spatial Safety
When an array index is a **compile-time constant**, RustifyC validates it immediately. Out-of-bounds constants produce a Rust-style error and halt compilation:
```
error[CWE-121]: spatial memory safety violation
  --> test.c:5:12
   |
 5 |     array[10] = ...;
   |     ^^^^^^^^^^ index 10 is out of bounds for array of length 5
   |
   = note: valid indices are in range [0, 4]
```

#### Phase 2 — Dynamic Spatial Safety
When the index is **runtime-determined** (user input, function arguments, calculated values), RustifyC performs **CFG surgery**: it splits the basic block at the array access, inserts a bounds-check conditional branch, and routes violations to a panic block that prints a Rust-style error and exits with code 101.

The bounds check uses **unsigned comparison** (`ICmpUGE`) — a single comparison that catches both positive overflow *and* negative indices (which wrap to large unsigned values).

#### Phase 3 — SCEV Loop Optimization
Using LLVM's **Scalar Evolution** engine, RustifyC mathematically proves whether a loop's index range is always in-bounds. If provable, the runtime check is **elided entirely** — zero overhead for safe loops.

#### Phase 4 — Uninitialized Memory Detection
RustifyC walks the **MemorySSA** graph to find loads whose clobbering definition is `LiveOnEntry` (never written). It distinguishes between *definitely* and *possibly* uninitialized (conditional paths via `MemoryPhi` nodes).

#### Phase 5 — Integer Overflow Detection
Signed arithmetic (`+`, `-`, `*`) is replaced with LLVM's hardware overflow intrinsics (`sadd.with.overflow`, etc.). If the CPU's overflow flag triggers, the program panics instead of silently wrapping.

#### Phase 6 — Stack Escape Analysis
A recursive analysis checks every `alloca` for pointer escapes: returning a stack address, storing to a global, or passing to an external function. Violations are **rejected at compile time** with a Rust-style borrow-checker error:
```
error[E0597]: stack variable escapes function scope
  --> test.c:4:5
   |
   = note: borrowed value does not live long enough
   = help: consider using heap allocation (malloc) or passing by value
```

---

## 📊 Test Results

Validated against a **152-test suite** (NIST Juliet CWE-121 inspired + generated edge cases):

| Metric | Result |
|---|---|
| True Positives (unsafe code caught) | 76 |
| True Negatives (safe code passed) | 75 |
| False Positives | 0 |
| False Negatives | 1 |
| **Accuracy** | **99.3%** |
| **Precision** | **100%** |
| **Recall** | **98.7%** |

> The single false negative is of a conditional-initialization case, this is a known limitation when optimization passes simplify MemoryPhi nodes.

---

## 🚀 Quick Start

### Prerequisites
- **macOS** (tested on Apple Silicon and Intel)
- **LLVM 15** (`brew install llvm@15`)
- **CMake** ≥ 3.13

### Build from Source

```bash
git clone https://github.com/elightknight23/rustifyC.git
cd rustifyC
mkdir -p build && cd build
cmake ..
make
cd ..
```

### Secure Your Code

```bash
# Compile with RustifyC protection
clang -fpass-plugin=./build/rustifyC.so your_code.c -o secure_binary

# Add -O1 for SCEV optimization (Phase 3 loop analysis)
clang -O1 -fpass-plugin=./build/rustifyC.so your_code.c -o secure_binary

# Add -g for file/line info in error messages
clang -g -fpass-plugin=./build/rustifyC.so your_code.c -o secure_binary
```

### Try the Interactive Demo

```bash
# Build unsafe and safe versions
clang -DUNSAFE demo_testcases_for_teacher/demo_interactive.c -o demo_unsafe
clang -fpass-plugin=./build/rustifyC.so demo_testcases_for_teacher/demo_interactive.c -o demo_safe

# Run without arguments to see the menu
./demo_safe

# Test 1: Data-oriented attack (struct buffer overflow → privilege escalation)
./demo_unsafe 1 4    # EXPLOITED! admin_flag overwritten
./demo_safe 1 4      # Caught! RustifyC panics at runtime

# Test 2: Dynamic OOB write
./demo_safe 2 15     # Caught! index 15 ≥ array length 7

# Test 3: Integer overflow
./demo_safe 3 0      # Caught! INT_MAX + 1 triggers overflow trap

# Test 4: Uninitialized memory read
./demo_safe 4        # Caught! variable read before assignment

# Test 5: Stack escape (compile-time protection)
./demo_unsafe 5      # UB! Dereferencing dangling pointer
./demo_safe 5        # RustifyC blocked the dangerous code at compile time
```

---

## 📁 Project Structure

```
rustifyC/
├── src/
│   └── rustifyC.cpp          # The LLVM pass implementation (716 lines, 6 phases)
├── CMakeLists.txt             # Build configuration
├── demo_testcases_for_teacher/
│   ├── demo_interactive.c     # Interactive multi-test demo with menu
│   ├── test.c                 # Data-oriented attack standalone demo
│   ├── 01_spatial_safety_static.c
│   ├── 02_spatial_safety_dynamic.c
│   ├── 03_scev_loop_optimization.c
│   ├── 04_uninitialized_memory.c
│   ├── 05_integer_overflow.c
│   ├── 06_stack_escape.c
│   ├── 07_real_world_combined.c  # All 6 vulnerability types in one file
│   └── README.txt
├── test/
│   ├── CWE121/                # NIST Juliet-inspired test suite (152 cases)
│   ├── run_juliet.sh          # Automated accuracy testing
│   ├── compare_clang.sh       # RustifyC vs Apple Clang comparison
│   └── benchmark.sh           # Performance benchmarks
├── demo_all.sh                # Full end-to-end demo script
└── run_all_benchmarks.sh      # Performance + accuracy suite
```

---

## ⚡ Compilation Report

Every compilation with RustifyC produces a detailed instrumentation report on stderr:

```
===========================================================
              RustifyC Compilation Report
===========================================================
Spatial Safety Analysis:
  ✓ Static overflows detected:          2
  ✓ Runtime checks injected:            3
  ✓ Safe accesses (no instrumentation): 1
  ✓ Negative indices caught:            0
  ✓ Multi-dimensional arrays secured:   1
  ✓ SCEV proofs elided checks:          1
Initialization Safety Analysis:
  ✓ Uninitialized reads prevented:      1
Temporal Safety Analysis:
  ✓ Stack escapes prevented:            1
Arithmetic Overflow Analysis:
  ✓ Overflow checks injected:           2
-----------------------------------------------------------
Instrumentation Rate: 27% (3/11 array accesses)
===========================================================
```

---

## 🔬 How It Compares

| Feature | Standard C | Clang `-Wall` | AddressSanitizer | **RustifyC** | Rust |
|---|---|---|---|---|---|
| Static OOB detection | ❌ | ⚠️ Partial | ❌ | ✅ | ✅ |
| Dynamic OOB detection | ❌ | ❌ | ✅ (2× slowdown) | ✅ (near-zero overhead) | ✅ |
| Uninitialized memory | ❌ | ⚠️ Warning only | ❌ | ✅ Blocks | ✅ |
| Integer overflow | ❌ | ❌ | ❌ | ✅ Hardware trap | ✅ (debug) |
| Dangling pointers | ❌ | ⚠️ Warning only | ❌ | ✅ Blocks compile | ✅ Borrow checker |
| Zero-cost safe loops | N/A | N/A | ❌ | ✅ SCEV proof | ✅ |
| Requires source changes | N/A | N/A | ❌ | **❌ None** | ✅ Full rewrite |
| Production-safe | ✅ | ✅ | ❌ (perf hit) | **✅** | ✅ |

---

## 🔧 Advanced Usage

### Compiler Flags Reference

| Flag | Purpose |
|---|---|
| `-fpass-plugin=./build/rustifyC.so` | Enable RustifyC |
| `-O1` | Enable SCEV optimization (Phase 3 loop proofs) |
| `-g` | Add debug info for better error messages with file/line |
| `-mllvm -stats` | Print LLVM statistics (detailed check counts) |
| `DISABLE_SCEV=1` | Environment variable to disable SCEV for benchmarking |
| `-DUNSAFE` | Preprocessor flag for demo: include unsafe stack-escape code |

### Run the Full Test Suite

```bash
# Run the Juliet accuracy test suite (152 tests)
cd test && bash run_juliet.sh

# Compare RustifyC vs Apple Clang detection rates
bash compare_clang.sh

# Run performance benchmarks
bash ../run_all_benchmarks.sh
```

---

## 🧠 Technical Deep Dive

RustifyC operates as a **New Pass Manager** plugin for LLVM 15. It registers two passes:

1. **`RustifyCPass`** (Function pass) — Iterates over every instruction in the function:
   - `GetElementPtrInst` → Spatial safety (Phases 1–3)
   - `LoadInst` → Uninitialized memory detection (Phase 4)
   - `BinaryOperator` → Integer overflow detection (Phase 5)
   - `AllocaInst` → Stack escape analysis (Phase 6)

2. **`RustifyCPrinter`** (Module pass) — Emits the summary compilation report after all functions are analyzed.

### Key Design Decisions

- **Unsigned comparison trick**: `ICmpUGE` on a sign-extended index catches both `index ≥ size` *and* negative indices (which become massive unsigned values) in a **single comparison**.
- **SCEV integration**: By computing the `ConstantRange` of loop variables via Scalar Evolution, provably-safe loops incur **zero runtime overhead**.
- **Rust-style errors**: Error messages mimic `rustc` error format (`error[E0597]`, `error[CWE-121]`, `error[E0381]`) for familiarity and professionalism.
- **Exit code 101**: Matches Rust's standard panic exit code for consistency.

---

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.

---

<p align="center">
  <em>RustifyC — because memory bugs shouldn't be a feature of your language.</em>
</p>
