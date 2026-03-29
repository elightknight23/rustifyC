// =============================================================
//  demo_interactive.c  —  RustifyC Interactive Demo
//  Usage:  ./demo_interactive <test_number> [optional_input]
//
//  Tests:
//    1  — Data-Oriented Attack (struct buffer overflow)
//    2  — Dynamic Spatial Safety (OOB write, user-controlled index)
//    3  — Integer Overflow (signed wrap-around)
//    4  — Uninitialized Memory Read (garbage value leak)
//    5  — Stack Escape / Dangling Pointer
//
//  Examples:
//    ./demo_interactive 1 4      -> test 1, attack at index 4
//    ./demo_interactive 2 15     -> test 2, OOB write at index 15
//    ./demo_interactive 2 3      -> test 2, safe write at index 3
//    ./demo_interactive 3        -> test 3, integer overflow
//    ./demo_interactive 4        -> test 4, uninitialized memory
//    ./demo_interactive 5        -> test 5, dangling pointer
// =============================================================

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// ─── Shared struct for test 1 ────────────────────────────────
struct vulnerable {
    int buffer[4];
    int admin_flag;
} vuln_data;

// ─── Test 5: dangling pointer helper ─────────────────────────
// Compiled with -DUNSAFE  -> the real dangling return is present (plain clang demo)
// Compiled without -DUNSAFE -> RustifyC would reject the escape; we skip the body
#ifdef UNSAFE
int* get_dangling_pointer() {
    // 'local_var' lives on the stack — destroyed when this function returns.
    // Returning its address creates a dangling pointer: UB in C.
    int local_var = 42;
    return &local_var;   // <-- triggers -Wreturn-stack-address and crashes at runtime
}
#endif

// ─────────────────────────────────────────────────────────────
//  TEST 1 — Data-Oriented Attack (struct layout corruption)
// ─────────────────────────────────────────────────────────────
void test1_data_oriented_attack(int user_index) {
    printf("\n=== TEST 1: Data-Oriented Attack ===\n");
    printf("Struct layout: buffer[0..3] sits right before admin_flag.\n");
    printf("Writing 1 to buffer[%d]...\n\n", user_index);

    // ── Tweak this value to show different outcomes ──
    // user_index = 0,1,2,3  -> in-bounds, flag stays 0  (safe)
    // user_index = 4        -> overwrites admin_flag!    (exploit!)
    // user_index = 5,6,...  -> OOB but misses flag (RustifyC still catches it)

    vuln_data.admin_flag = 0;
    vuln_data.buffer[user_index] = 1;

    if (vuln_data.admin_flag) {
        printf("CRITICAL: admin access GRANTED (flag = %d)\n", vuln_data.admin_flag);
        printf("Privilege escalation successful!\n");
    } else {
        printf("Access to admin denied (flag = %d)\n", vuln_data.admin_flag);
        printf("Success!\n");
    }
}

// ─────────────────────────────────────────────────────────────
//  TEST 2 — Dynamic Spatial Safety (plain OOB array write)
// ─────────────────────────────────────────────────────────────
void test2_dynamic_oob(int index) {
    printf("\n=== TEST 2: Dynamic Spatial Safety ===\n");
    printf("Buffer size: 7 elements (valid indices: 0-6)\n");
    printf("Attempting to write to buffer[%d]...\n\n", index);

    // ── Tweak this value to show different outcomes ──
    // index = 0..6   -> fine, write succeeds
    // index = 7+     -> RustifyC panics; plain C silently corrupts memory
    // index = -1     -> negative index: RustifyC catches this too!
    int buffer[7];
    buffer[index] = 42;
    printf("Wrote 42 to buffer[%d]. Read back: %d\n", index, buffer[index]);
    printf("No memory corruption occurred.\n");
}

// ─────────────────────────────────────────────────────────────
//  TEST 3 — Integer Overflow (signed wrap-around)
// ─────────────────────────────────────────────────────────────
void test3_integer_overflow(int extra) {
    printf("\n=== TEST 3: Integer Overflow ===\n");
    printf("INT_MAX = %d\n", INT_MAX);

    // ── Tweak 'extra' to control the overflow magnitude ──
    // extra = 0  -> INT_MAX + 1                  (classic wrap: becomes INT_MIN)
    // extra = 5  -> INT_MAX + 6                  (deeper overflow)
    // Without RustifyC this just silently wraps. With RustifyC it panics.
    int big = INT_MAX;
    printf("Attempting: INT_MAX + %d + 1...\n\n", extra);
    int result = big + extra + 1;   // <-- RustifyC injects overflow check here
    printf("You should NOT see this if RustifyC is active: %d\n", result);
}

// ─────────────────────────────────────────────────────────────
//  TEST 4 — Uninitialized Memory Read (data leak)
// ─────────────────────────────────────────────────────────────
void test4_uninitialized_memory() {
    printf("\n=== TEST 4: Uninitialized Memory Read ===\n");
    printf("Declaring 'secret' without assigning a value...\n\n");

    // ── Nothing to tweak — the point is 'secret' is never assigned ──
    // In plain C, this prints whatever garbage was on the stack — could be
    // a password or key from a previous stack frame! RustifyC traps this.
    int secret;
    printf("Leaked value from uninitialized memory: %d\n", secret);
    printf("(In real attacks, this could be a password or key.)\n");
}

// ─────────────────────────────────────────────────────────────
//  TEST 5 — Stack Escape / Dangling Pointer
// ─────────────────────────────────────────────────────────────
void test5_stack_escape() {
    printf("\n=== TEST 5: Stack Escape / Dangling Pointer ===\n");

#ifdef UNSAFE
    // ── UNSAFE build: the real exploit runs ──
    // get_dangling_pointer() returns the address of a local that is already
    // destroyed. Dereferencing it is undefined behaviour — garbage or crash.
    printf("[UNSAFE] Calling get_dangling_pointer()...\n\n");
    int* bad_ptr = get_dangling_pointer();
    printf("Dereferencing dangling pointer: %d\n", *bad_ptr);
    printf("(Value is garbage — the stack frame is gone!)\n");
#else
    // ── SAFE build (RustifyC): the stack escape is blocked at compile time ──
    // When you try to compile this WITHOUT -DUNSAFE and WITH the RustifyC
    // plugin, the compiler emits:
    //   error[E0597]: stack variable escapes function scope
    // ...and refuses to produce a binary at all.
    printf("[SAFE]   RustifyC blocked this at COMPILE TIME.\n");
    printf("         The binary was never produced for the unsafe version.\n");
    printf("         Try: clang -DUNSAFE demo_interactive.c -o demo_unsafe\n");
    printf("              ./demo_unsafe 5\n");
    printf("         to see the raw undefined behaviour.\n");
#endif
}

// ─────────────────────────────────────────────────────────────
//  MAIN — menu + optional second argument as numeric input
// ─────────────────────────────────────────────────────────────
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("RustifyC Interactive Demo\n");
        printf("Usage: %s <test> [value]\n\n", argv[0]);
        printf("  1  Data-Oriented Attack       (value = array index, try 0-6)\n");
        printf("  2  Dynamic OOB Write          (value = array index, try 0-10)\n");
        printf("  3  Integer Overflow           (value = extra addend, try 0-5)\n");
        printf("  4  Uninitialized Memory Read  (no value needed)\n");
        printf("  5  Stack Escape / Dangling Ptr (no value needed)\n");
        return 1;
    }

    int test  = atoi(argv[1]);
    int value = argc >= 3 ? atoi(argv[2]) : 0;

    switch (test) {
        case 1: test1_data_oriented_attack(value); break;
        case 2: test2_dynamic_oob(value);          break;
        case 3: test3_integer_overflow(value);     break;
        case 4: test4_uninitialized_memory();      break;
        case 5: test5_stack_escape();              break;
        default:
            printf("Unknown test '%d'. Choose 1-5.\n", test);
            return 1;
    }

    return 0;
}
