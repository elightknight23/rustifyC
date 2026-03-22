import os
import random

OUT_DIR = "test/CWE121"
os.makedirs(OUT_DIR, exist_ok=True)

test_count = 0

def write_test(name, expected, content):
    global test_count
    # Add _bad_ or _good_ appropriately for run_juliet.sh
    suffix = "bad" if expected == "UNSAFE" else "good"
    filename = f"{OUT_DIR}/gen_{name:03d}_{expected.lower()}_{suffix}.c"
    with open(filename, "w") as f:
        f.write("#include <stdio.h>\n")
        f.write("#include <stdlib.h>\n")
        f.write("#include <limits.h>\n\n")
        f.write(content)
    test_count += 1

# Phase 1 & 2: Spatial Safety
for i in range(20):
    size = random.randint(5, 50)
    
    # 10 UNSAFE
    write_test(test_count, "UNSAFE", f"""
int main() {{
    int arr[{size}];
    arr[{size + random.randint(0, 10)}] = 42; // static OOB
    return 0;
}}""")
    
    # 10 SAFE
    write_test(test_count, "SAFE", f"""
int main() {{
    int arr[{size}];
    arr[{random.randint(0, size-1)}] = 42; // static safe
    return 0;
}}""")

for i in range(15):
    size = random.randint(10, 100)
    # UNSAFE Dynamic Loop
    write_test(test_count, "UNSAFE", f"""
int main(int argc, char **argv) {{
    int arr[{size}];
    for(int i = 0; i <= {size}; i++) {{ // off by one
        arr[i] = i; 
    }}
    return 0;
}}""")
    # SAFE Dynamic Loop
    write_test(test_count, "SAFE", f"""
int main(int argc, char **argv) {{
    int arr[{size}];
    for(int i = 0; i < {size}; i++) {{ // safe
        arr[i] = i; 
    }}
    return 0;
}}""")

# Phase 4: Uninitialized
for i in range(10):
    write_test(test_count, "UNSAFE", f"""
int main() {{
    int _var{i};
    printf("%d\\n", _var{i}); // uninit
    return 0;
}}""")
    write_test(test_count, "SAFE", f"""
int main() {{
    int _var{i} = {i};
    printf("%d\\n", _var{i}); // init
    return 0;
}}""")

# Phase 5: Overflow
for i in range(10):
    write_test(test_count, "UNSAFE", f"""
int main(int argc, char **argv) {{
    int x = INT_MAX - {i} + argc + 50; // overflow
    printf("%d\\n", x);
    return 0;
}}""")
    write_test(test_count, "SAFE", f"""
int main(int argc, char **argv) {{
    int x = 1000 + {i} * argc; // safe
    printf("%d\\n", x);
    return 0;
}}""")

# Phase 6: Stack Escape
for i in range(10):
    write_test(test_count, "UNSAFE", f"""
int* escape{i}() {{
    int local = {i};
    return &local;
}}
int main() {{
    int *p = escape{i}();
    return 0;
}}""")
    write_test(test_count, "SAFE", f"""
int escape{i}() {{
    int local = {i};
    return local;
}}
int main() {{
    int p = escape{i}();
    return 0;
}}""")

print(f"Generated {test_count} new test cases in {OUT_DIR}/")
