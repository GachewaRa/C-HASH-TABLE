#include <stdio.h>   // For printf
#include <stdlib.h>  // For malloc

int main() {
    printf("Program starting: Allocating memory...\n");

    // Allocate memory for 100 integers
    int *data = (int *)malloc(100 * sizeof(int));

    // Check if malloc succeeded (good practice!)
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1; // Indicate an error
    }

    // Use the allocated memory (e.g., initialize it)
    for (int i = 0; i < 100; i++) {
        data[i] = i * 2;
    }

    printf("Memory allocated and used. Now exiting WITHOUT freeing it.\n");

    // FORGETTING TO FREE 'data' HERE IS THE MEMORY LEAK
    // free(data); // If this line were uncommented, there would be no leak

    return 0; // Program exits normally
}
