#include <stdio.h> // For printf
#include <stdlib.h> // For NULL

int main() {
    // 1. Create a pointer to an integer
    int *ptr;

    // 2. Set the pointer to NULL
    ptr = NULL;

    printf("Attempting to dereference a NULL pointer...\n");

    // 3. Try to dereference it and assign a value
    // THIS LINE WILL CAUSE A SEGMENTATION FAULT
    *ptr = 100;

    // This line will typically not be reached
    printf("This line should not be printed if a segmentation fault occurs.\n");

    return 0; // This line will typically not be reached
}

// gcc -g -o null null.c
// gdb null
// (gdb) run