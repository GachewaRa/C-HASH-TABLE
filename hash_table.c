/**
 * Complete Hash Table Implementation in C
 * 
 * This implementation provides a generic hash table that can store key-value
 * pairs with string keys and values of any type (using void pointers).
 * It handles collisions using linked lists (chaining).
 */

//1. Required Header Files
#include <stdio.h>      // For standard I/O operations
#include <stdlib.h>     // For memory allocation (malloc, free)
#include <string.h>     // For string operations (strcmp, strdup)
#include <stdbool.h>    // For boolean data type (true, false)

/**
 * KeyValuePair Structure
 * 
 * This structure represents a single key-value pair in our hash table.
 * We use a linked list approach to handle collisions (when multiple keys hash to the same bucket).
 */
typedef struct KeyValuePair {
    char* key;                  // The string key (we store a copy of the original)
    void* value;                // A pointer to the value (can be any data type)
    struct KeyValuePair* next;  // Pointer to the next KeyValuePair in case of collision
} KeyValuePair;

/**
 * HashTable Structure
 * 
 * The main hash table structure that contains the array of buckets.
 * Each bucket is a pointer to a potential linked list of KeyValuePair elements.
 */
typedef struct HashTable {
    int capacity;       // The number of buckets in the hash table
    KeyValuePair** array; // Array of pointers to KeyValuePair (the buckets)
    int size;           // The current number of elements stored in the hash table
} HashTable;

/**
 * Hash Function (djb2 algorithm)
 * 
 * This function converts a string key into a numeric hash value.
 * A good hash function distributes keys uniformly across the hash table.
 * 
 * @param key The string key to hash
 * @return The numeric hash value
 */
unsigned long hash(const char* key) {
    unsigned long hash = 5381;  // Start with a prime number
    int c;
    
    // For each character in the key, update the hash value
    while ((c = *key++)) {
        // This operation: hash * 33 + c
        // (written as ((hash << 5) + hash) + c for efficiency)
        // combines the current hash with the next character in a way
        // that distributes changes throughout the bits of the hash
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/**
 * Get the index in the hash table's array
 * 
 * This function converts a hash value to an index within the capacity of our table.
 * We use the modulo operation to ensure the index is within our array bounds.
 * 
 * @param ht The hash table
 * @param key The string key to find the index for
 * @return The index in the hash table array where this key belongs
 */
int getIndex(HashTable* ht, const char* key) {
    unsigned long hashValue = hash(key);
    return hashValue % ht->capacity;  // Ensure index is within array bounds
}

/**
 * Create a new hash table
 * 
 * Allocates memory for a new hash table with the specified capacity
 * and initializes all buckets to NULL.
 * 
 * @param capacity The number of buckets in the hash table
 * @return A pointer to the newly created hash table, or NULL if allocation fails
 */
HashTable* createHashTable(int capacity) {
    // Allocate memory for the hash table structure
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;  // Memory allocation failed
    }

    // Initialize the hash table fields
    ht->capacity = capacity;
    ht->size = 0;
    
    // Allocate memory for the array of buckets
    ht->array = (KeyValuePair**)malloc(capacity * sizeof(KeyValuePair*));
    if (ht->array == NULL) {
        free(ht);  // Clean up if allocation fails
        return NULL;
    }

    // Initialize all buckets to NULL (empty)
    for (int i = 0; i < capacity; i++) {
        ht->array[i] = NULL;
    }

    return ht;
}

/**
 * Insert a key-value pair into the hash table
 * 
 * If the key already exists, its value is updated.
 * Otherwise, a new key-value pair is created.
 * 
 * @param ht The hash table
 * @param key The string key
 * @param value Pointer to the value to store
 * @return true if insertion was successful, false otherwise
 */
bool insert(HashTable* ht, const char* key, void* value) {
    // Calculate which bucket this key belongs in
    int index = getIndex(ht, key);

    // Check if the key already exists in the table
    KeyValuePair* current = ht->array[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Key found: update the value and return
            current->value = value;
            return true;
        }
        current = current->next;
    }

    // Key doesn't exist: create a new key-value pair
    KeyValuePair* newPair = (KeyValuePair*)malloc(sizeof(KeyValuePair));
    if (newPair == NULL) {
        return false;  // Memory allocation failed
    }
    
    // Store a copy of the key (important to avoid issues if original key is modified or freed)
    newPair->key = strdup(key);
    if (newPair->key == NULL) {
        free(newPair);  // Clean up if strdup fails
        return false;
    }
    
    // Set the value and link this pair at the beginning of the bucket's list
    newPair->value = value;
    newPair->next = ht->array[index];  // The current head becomes the next of our new pair
    ht->array[index] = newPair;        // The new pair becomes the new head
    ht->size++;                        // Increment the total size
    
    return true;
}

/**
 * Retrieve a value from the hash table by its key
 * 
 * @param ht The hash table
 * @param key The key to look up
 * @return The value associated with the key, or NULL if key not found
 */
void* get(HashTable* ht, const char* key) {
    // Calculate which bucket this key would be in
    int index = getIndex(ht, key);
    
    // Traverse the linked list in this bucket to find the key
    KeyValuePair* current = ht->array[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Key found: return its value
            return current->value;
        }
        current = current->next;
    }
    
    // Key not found
    return NULL;
}

/**
 * Delete a key-value pair from the hash table
 * 
 * @param ht The hash table
 * @param key The key to delete
 * @return true if key was found and deleted, false if key not found
 */
bool delete(HashTable* ht, const char* key) {
    // Calculate which bucket this key would be in
    int index = getIndex(ht, key);
    
    KeyValuePair* current = ht->array[index];
    KeyValuePair* prev = NULL;

    // Traverse the linked list to find the key
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            // Key found: remove this node from the linked list
            if (prev == NULL) {
                // This is the first node in the list
                ht->array[index] = current->next;
            } else {
                // This is not the first node
                prev->next = current->next;
            }
            
            // Free the memory used by this key-value pair
            free(current->key);  // Free the duplicated key string
            free(current);       // Free the KeyValuePair structure
            ht->size--;          // Decrease the total size
            
            return true;  // Successfully deleted
        }
        
        // Move to the next node
        prev = current;
        current = current->next;
    }
    
    // Key not found
    return false;
}

/**
 * Free all memory used by the hash table
 * 
 * @param ht The hash table to free
 */
void freeHashTable(HashTable* ht) {
    if (ht == NULL) return;

    // Free all KeyValuePair nodes in all buckets
    for (int i = 0; i < ht->capacity; i++) {
        KeyValuePair* current = ht->array[i];
        while (current != NULL) {
            KeyValuePair* next = current->next;
            free(current->key);  // Free the key string
            free(current);       // Free the KeyValuePair structure
            current = next;
        }
    }
    
    // Free the array of buckets and the hash table structure itself
    free(ht->array);
    free(ht);
}

/**
 * Print the contents of the hash table (for debugging)
 * 
 * @param ht The hash table to print
 */
void printHashTable(HashTable* ht) {
    printf("Hash Table (size: %d, capacity: %d)\n", ht->size, ht->capacity);
    
    for (int i = 0; i < ht->capacity; i++) {
        KeyValuePair* current = ht->array[i];
        if (current != NULL) {
            printf("  Bucket %d:", i);
            while (current != NULL) {
                printf(" [%s]->", current->key);
                current = current->next;
            }
            printf("NULL\n");
        }
    }
}

/**
 * Example of hash table usage
 */
/*
int main() {
    // Create a hash table with 10 buckets
    HashTable* ht = createHashTable(10);
    
    // Define some example values (normally these would be more complex data structures)
    int value1 = 100;
    int value2 = 200;
    int value3 = 300;
    
    // Insert key-value pairs
    insert(ht, "key1", &value1);
    insert(ht, "key2", &value2);
    insert(ht, "key3", &value3);
    
    // Print the hash table
    printHashTable(ht);
    
    // Retrieve and print a value
    int* retrieved = (int*)get(ht, "key2");
    if (retrieved != NULL) {
        printf("Value for key2: %d\n", *retrieved);
    }
    
    // Delete a key
    if (delete(ht, "key1")) {
        printf("Deleted key1\n");
    }
    
    // Print the hash table again to see the change
    printHashTable(ht);
    
    // Free the hash table
    freeHashTable(ht);
    
    return 0;
}
*/