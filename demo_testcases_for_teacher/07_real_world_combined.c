// real_world_combined.c
// ---------------------------------------------------------------
// We are simulating a tiny "Student Grade Management System"
// that you might actually write for a CS assignment.
// It looks normal on the surface, but it has EVERY kind of
// memory bug that RustifyC is designed to catch.
// ---------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_STUDENTS 10

// -- VULNERABILITY 6: Stack Escape --
// This function is supposed to return a pointer to a new record.
// Problem: 'new_student' is created on the stack of this function.
// The moment the function returns, that memory is GONE.
// Returning its address creates a dangling pointer.
// RustifyC's Phase 6 (stack escape detection) will catch this!
int* create_student_record(int id, int grade) {
    int new_student[2];  // local array: id at [0], grade at [1]
    new_student[0] = id;
    new_student[1] = grade;
    return &new_student[0]; // BUG: returning address of a local variable
}

// -- VULNERABILITY 2: Dynamic Bounds --
// We let the user look up a student by typing in an index.
// We never verify that the number they type is actually within our array.
// A user typing "999" would read totally random memory!
// RustifyC's Phase 2 (dynamic bounds injection) will catch this!
void lookup_student(int* registry, int user_index) {
    printf("Grade for student at index %d: %d\n", user_index, registry[user_index]);
}

// -- VULNERABILITY 3: SCEV Optimization (for contrast) --
// This loop is 100%% safe. 'i' goes from 0 to MAX_STUDENTS - 1.
// RustifyC's Phase 3 (SCEV math) will PROVE it's safe and 
// remove any injected checks, so we get zero overhead here!
void print_all_grades(int* registry, int count) {
    for (int i = 0; i < count; i++) {
        printf("Student %d -> Grade: %d\n", i, registry[i]);
    }
}

// -- VULNERABILITY 5: Integer Overflow --
// This function adds up all the grades to calculate a total score.
// If somehow the grades are huge (like INT_MAX), the sum wraps negative.
// In a real grading app, this could give students a negative total score!
// RustifyC's Phase 5 (overflow detection) will catch this!
int sum_grades(int* registry, int count, int bonus) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += registry[i];
    }
    // Teacher: If bonus is INT_MAX, this final addition overflows silently in normal C.
    total = total + bonus;
    return total;
}

// -- VULNERABILITY 4: Uninitialized Memory --
// This "calculates" the average, but we declared 'average_grade'
// without ever setting it. If the count is 0, we fall through
// and print whatever garbage was in memory where 'average_grade' lives.
// RustifyC's Phase 4 (uninitialized memory detection) will catch this!
void report_average(int* registry, int count) {
    int average_grade; // FORGOT to initialize this to 0!
    if (count > 0) {
        int sum = 0;
        for (int i = 0; i < count; i++) sum += registry[i];
        average_grade = sum / count;
    }
    // If count == 0, average_grade is never set, but we print it anyway!
    printf("Class Average: %d\n", average_grade);
}

int main(int argc, char** argv) {
    // Our 10-student grade registry
    int grade_registry[MAX_STUDENTS] = {85, 92, 78, 95, 60, 88, 73, 91, 55, 82};

    // -- VULNERABILITY 1: Static Bounds --
    // We have 10 students (indices 0-9), but here we write to index 10 by mistake.
    // This is a classic off-by-one error that any junior developer might write.
    // RustifyC's Phase 1 (static diagnostics) will catch this at COMPILE TIME!
    grade_registry[10] = 100; // BUG: index 10 is out of bounds for size 10

    printf("=== Student Grade Management System ===\n");

    // Print all grades (this is the SAFE loop that SCEV will optimize)
    print_all_grades(grade_registry, MAX_STUDENTS);

    // Lookup by user input (dynamic bounds danger!)
    // Teacher: If you pass argv[1] as say "50", it reads random memory!
    if (argc > 1) {
        int lookup_idx = atoi(argv[1]);
        lookup_student(grade_registry, lookup_idx);
    }

    // Sum grades with a large bonus -> integer overflow risk
    int total = sum_grades(grade_registry, MAX_STUDENTS, INT_MAX);
    printf("Total Score with Bonus: %d\n", total);

    // Print average -> uninitialized memory risk if count is 0
    report_average(grade_registry, 0); // passing 0 triggers the bug

    // Create a "new" student record -> stack escape bug
    int* new_record = create_student_record(11, 97);
    printf("New student grade: %d\n", *new_record); // dangling pointer access!

    return 0;
}
