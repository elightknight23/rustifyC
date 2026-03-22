import os
import random

OUT_DIR = "test/CWE121"
os.makedirs(OUT_DIR, exist_ok=True)

test_count = 200 # start offset

def write_test(name, expected, content):
    global test_count
    suffix = "bad" if expected == "UNSAFE" else "good"
    filename = f"{OUT_DIR}/genC_{name:03d}_{expected.lower()}_{suffix}.c"
    with open(filename, "w") as f:
        f.write("#include <stdio.h>\n")
        f.write("#include <stdlib.h>\n")
        f.write("#include <limits.h>\n\n")
        f.write(content)
    test_count += 1

# We generate 100 UNSAFE tests that Clang typically misses (dynamic OOB, uninit, escape, overflow)
for i in range(25):
    size = random.randint(10, 500)
    write_test(test_count, "UNSAFE", f"""
int main(int argc, char **argv) {{
    int arr[{size}];
    int idx = argc + {size}; 
    arr[idx] = 42; // dynamic OOB
    return 0;
}}""")

for i in range(25):
    write_test(test_count, "UNSAFE", f"""
int main(int argc, char **argv) {{
    int _var{i};
    if (argc > 10) _var{i} = 1;
    printf("%d\\n", _var{i}); // conditionally uninit
    return 0;
}}""")

for i in range(25):
    write_test(test_count, "UNSAFE", f"""
int main(int argc, char **argv) {{
    int x = INT_MAX - {i} + argc * 100; // overflow
    printf("%d\\n", x);
    return 0;
}}""")

for i in range(25):
    write_test(test_count, "UNSAFE", f"""
int* escape{i}() {{
    int local[10];
    local[0] = {i};
    return local; // stack escape
}}
int main() {{
    int *p = escape{i}();
    return p[0];
}}""")

print(f"Generated 100 additional Clang-comparison test cases.")
