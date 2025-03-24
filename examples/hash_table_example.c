/**
 * @file hash_table_example.c
 * @brief Example usage of myrtx hash table
 */

#include "myrtx/collections/hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Example of using hash table with string keys */
void string_keys_example(void) {
    myrtx_arena_t arena = {0};
    myrtx_hash_table_t* table;
    void* value_ptr;
    size_t value_size;
    
    printf("=== String Keys Example ===\n");
    
    /* Initialize arena */
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Failed to initialize arena\n");
        return;
    }
    
    /* Create hash table with string keys */
    table = myrtx_hash_table_create(&arena, 16, myrtx_hash_string, myrtx_compare_string_keys);
    if (!table) {
        printf("Failed to create hash table\n");
        myrtx_arena_free(&arena);
        return;
    }
    
    /* Insert key-value pairs */
    printf("Inserting key-value pairs...\n");
    
    int value1 = 42;
    if (!myrtx_hash_table_put(table, "answer", 0, &value1, sizeof(int))) {
        printf("Failed to insert 'answer'\n");
    }
    
    double value2 = 3.14159;
    if (!myrtx_hash_table_put(table, "pi", 0, &value2, sizeof(double))) {
        printf("Failed to insert 'pi'\n");
    }
    
    const char* value3 = "Hello, world!";
    if (!myrtx_hash_table_put(table, "greeting", 0, value3, strlen(value3) + 1)) {
        printf("Failed to insert 'greeting'\n");
    }
    
    /* Query the hash table */
    printf("\nQuerying the hash table:\n");
    
    if (myrtx_hash_table_contains_key(table, "answer", 0)) {
        printf("Key 'answer' exists\n");
        
        if (myrtx_hash_table_get(table, "answer", 0, &value_ptr, &value_size)) {
            printf("Value: %d\n", *(int*)value_ptr);
        }
    }
    
    if (myrtx_hash_table_contains_key(table, "pi", 0)) {
        printf("Key 'pi' exists\n");
        
        if (myrtx_hash_table_get(table, "pi", 0, &value_ptr, &value_size)) {
            printf("Value: %f\n", *(double*)value_ptr);
        }
    }
    
    if (myrtx_hash_table_contains_key(table, "greeting", 0)) {
        printf("Key 'greeting' exists\n");
        
        if (myrtx_hash_table_get(table, "greeting", 0, &value_ptr, &value_size)) {
            printf("Value: %s\n", (char*)value_ptr);
        }
    }
    
    if (!myrtx_hash_table_contains_key(table, "nonexistent", 0)) {
        printf("Key 'nonexistent' does not exist\n");
    }
    
    /* Update a value */
    printf("\nUpdating a value...\n");
    
    int new_value = 100;
    if (myrtx_hash_table_put(table, "answer", 0, &new_value, sizeof(int))) {
        printf("Updated 'answer'\n");
        
        if (myrtx_hash_table_get(table, "answer", 0, &value_ptr, &value_size)) {
            printf("New value: %d\n", *(int*)value_ptr);
        }
    }
    
    /* Remove a key */
    printf("\nRemoving a key...\n");
    
    if (myrtx_hash_table_remove(table, "pi", 0, false, false)) {
        printf("Removed 'pi'\n");
    }
    
    if (!myrtx_hash_table_contains_key(table, "pi", 0)) {
        printf("Key 'pi' no longer exists\n");
    }
    
    /* Get current size */
    printf("\nCurrent hash table size: %zu\n", myrtx_hash_table_size(table));
    
    /* Clean up */
    myrtx_hash_table_free(table, false, false);  /* No need to free when using arena */
    myrtx_arena_free(&arena);
    
    printf("String keys example completed\n\n");
}

/* Example of using hash table with integer keys */
void integer_keys_example(void) {
    myrtx_arena_t arena = {0};
    myrtx_hash_table_t* table;
    void* value_ptr;
    size_t value_size;
    
    printf("=== Integer Keys Example ===\n");
    
    /* Initialize arena */
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Failed to initialize arena\n");
        return;
    }
    
    /* Create hash table with integer keys */
    table = myrtx_hash_table_create(&arena, 16, myrtx_hash_integer, myrtx_compare_integer_keys);
    if (!table) {
        printf("Failed to create hash table\n");
        myrtx_arena_free(&arena);
        return;
    }
    
    /* Insert key-value pairs */
    printf("Inserting key-value pairs with integer keys...\n");
    
    int keys[5] = {10, 20, 30, 40, 50};
    
    const char* value1 = "Ten";
    if (!myrtx_hash_table_put(table, &keys[0], sizeof(int), value1, strlen(value1) + 1)) {
        printf("Failed to insert key 10\n");
    }
    
    const char* value2 = "Twenty";
    if (!myrtx_hash_table_put(table, &keys[1], sizeof(int), value2, strlen(value2) + 1)) {
        printf("Failed to insert key 20\n");
    }
    
    const char* value3 = "Thirty";
    if (!myrtx_hash_table_put(table, &keys[2], sizeof(int), value3, strlen(value3) + 1)) {
        printf("Failed to insert key 30\n");
    }
    
    /* Query the hash table */
    printf("\nQuerying the hash table:\n");
    
    for (int i = 0; i < 5; i++) {
        if (myrtx_hash_table_contains_key(table, &keys[i], sizeof(int))) {
            printf("Key %d exists\n", keys[i]);
            
            if (myrtx_hash_table_get(table, &keys[i], sizeof(int), &value_ptr, &value_size)) {
                printf("Value: %s\n", (char*)value_ptr);
            }
        } else {
            printf("Key %d does not exist\n", keys[i]);
        }
    }
    
    /* Clean up */
    myrtx_hash_table_free(table, false, false);  /* No need to free when using arena */
    myrtx_arena_free(&arena);
    
    printf("Integer keys example completed\n\n");
}

/* Example of using hash table without arena (with malloc/free) */
void without_arena_example(void) {
    myrtx_hash_table_t* table;
    void* value_ptr;
    size_t value_size;
    
    printf("=== Without Arena Example ===\n");
    
    /* Create hash table without arena */
    table = myrtx_hash_table_create(NULL, 16, myrtx_hash_string, myrtx_compare_string_keys);
    if (!table) {
        printf("Failed to create hash table\n");
        return;
    }
    
    /* Insert key-value pairs */
    printf("Inserting key-value pairs...\n");
    
    int value1 = 42;
    if (!myrtx_hash_table_put(table, "answer", 0, &value1, sizeof(int))) {
        printf("Failed to insert 'answer'\n");
    }
    
    double value2 = 3.14159;
    if (!myrtx_hash_table_put(table, "pi", 0, &value2, sizeof(double))) {
        printf("Failed to insert 'pi'\n");
    }
    
    /* Query the hash table */
    printf("\nQuerying the hash table:\n");
    
    if (myrtx_hash_table_contains_key(table, "answer", 0)) {
        printf("Key 'answer' exists\n");
        
        if (myrtx_hash_table_get(table, "answer", 0, &value_ptr, &value_size)) {
            printf("Value: %d\n", *(int*)value_ptr);
        }
    }
    
    /* Clean up with key and value freeing */
    printf("\nCleaning up...\n");
    myrtx_hash_table_free(table, true, true);
    
    printf("Without arena example completed\n\n");
}

/* Example of stress testing the hash table with many entries */
void stress_test_example(void) {
    myrtx_arena_t arena = {0};
    myrtx_hash_table_t* table;
    char key_buffer[32];
    int value = 0;
    void* value_ptr;
    size_t value_size;
    const int NUM_ENTRIES = 10000;
    
    printf("=== Stress Test Example ===\n");
    
    /* Initialize arena */
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Failed to initialize arena\n");
        return;
    }
    
    /* Create hash table */
    table = myrtx_hash_table_create(&arena, 16, myrtx_hash_string, myrtx_compare_string_keys);
    if (!table) {
        printf("Failed to create hash table\n");
        myrtx_arena_free(&arena);
        return;
    }
    
    /* Insert many entries */
    printf("Inserting %d entries...\n", NUM_ENTRIES);
    
    for (int i = 0; i < NUM_ENTRIES; i++) {
        sprintf(key_buffer, "key_%d", i);
        value = i;
        
        if (!myrtx_hash_table_put(table, key_buffer, 0, &value, sizeof(int))) {
            printf("Failed to insert key '%s'\n", key_buffer);
            break;
        }
    }
    
    /* Query some random entries */
    printf("\nQuerying some entries:\n");
    
    for (int i = 0; i < 5; i++) {
        int index = rand() % NUM_ENTRIES;
        sprintf(key_buffer, "key_%d", index);
        
        if (myrtx_hash_table_get(table, key_buffer, 0, &value_ptr, &value_size)) {
            printf("Key '%s' = %d\n", key_buffer, *(int*)value_ptr);
        }
    }
    
    /* Final stats */
    printf("\nFinal hash table size: %zu\n", myrtx_hash_table_size(table));
    
    /* Clean up */
    myrtx_hash_table_free(table, false, false);
    myrtx_arena_free(&arena);
    
    printf("Stress test completed\n\n");
}

int main(void) {
    printf("=== Hash Table Example ===\n\n");
    
    string_keys_example();
    integer_keys_example();
    without_arena_example();
    stress_test_example();
    
    printf("All examples completed successfully\n");
    return 0;
} 