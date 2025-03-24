/**
 * @file hash_table_test.c
 * @brief Tests for the myrtx hash table implementation
 */

#include "myrtx/collections/hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_PASSED() printf("PASSED: %s\n", __func__)
#define TEST_FAILED(msg) do { printf("FAILED: %s - %s\n", __func__, msg); exit(1); } while(0)

/* Test creating and freeing hash tables */
void test_create_free(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test with arena */
    myrtx_hash_table_t* table1 = myrtx_hash_table_create(&arena, 16, 
                                                       myrtx_hash_string, 
                                                       myrtx_compare_string_keys);
    if (!table1) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table with arena");
    }
    
    /* Test with NULL arena (using malloc) */
    myrtx_hash_table_t* table2 = myrtx_hash_table_create(NULL, 16, 
                                                       myrtx_hash_string, 
                                                       myrtx_compare_string_keys);
    if (!table2) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table with malloc");
    }
    
    /* Test with other hash functions */
    myrtx_hash_table_t* table3 = myrtx_hash_table_create(&arena, 16, 
                                                       myrtx_hash_integer, 
                                                       myrtx_compare_integer_keys);
    if (!table3) {
        myrtx_hash_table_free(table2, false, false);
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table with integer hash");
    }
    
    /* Test default capacity */
    myrtx_hash_table_t* table4 = myrtx_hash_table_create(&arena, 0, 
                                                       myrtx_hash_string, 
                                                       myrtx_compare_string_keys);
    if (!table4) {
        myrtx_hash_table_free(table2, false, false);
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table with default capacity");
    }
    
    /* Test invalid parameters */
    myrtx_hash_table_t* table5 = myrtx_hash_table_create(&arena, 16, 
                                                       NULL, 
                                                       myrtx_compare_string_keys);
    if (table5) {
        myrtx_hash_table_free(table2, false, false);
        myrtx_arena_free(&arena);
        TEST_FAILED("Created hash table with NULL hash function");
    }
    
    myrtx_hash_table_t* table6 = myrtx_hash_table_create(&arena, 16, 
                                                       myrtx_hash_string, 
                                                       NULL);
    if (table6) {
        myrtx_hash_table_free(table2, false, false);
        myrtx_arena_free(&arena);
        TEST_FAILED("Created hash table with NULL compare function");
    }
    
    /* Free the tables (no need to free when using arena except table2) */
    myrtx_hash_table_free(table2, false, false);
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

/* Test basic put, get, and contains operations */
void test_basic_operations(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 16, 
                                                      myrtx_hash_string, 
                                                      myrtx_compare_string_keys);
    if (!table) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table");
    }
    
    /* Test initial state */
    if (myrtx_hash_table_size(table) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Initial size is not 0");
    }
    
    if (myrtx_hash_table_contains_key(table, "key1", 0)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Contains returned true for nonexistent key");
    }
    
    /* Test put and get */
    int value1 = 42;
    if (!myrtx_hash_table_put(table, "key1", 0, &value1, sizeof(int))) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to put key1");
    }
    
    if (myrtx_hash_table_size(table) != 1) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size is not 1 after putting one key");
    }
    
    if (!myrtx_hash_table_contains_key(table, "key1", 0)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Contains returned false for existing key");
    }
    
    void* out_value;
    size_t out_size;
    if (!myrtx_hash_table_get(table, "key1", 0, &out_value, &out_size)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to get key1");
    }
    
    if (out_size != sizeof(int) || *(int*)out_value != value1) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Retrieved value does not match put value");
    }
    
    /* Test updating a value */
    int value2 = 100;
    if (!myrtx_hash_table_put(table, "key1", 0, &value2, sizeof(int))) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to update key1");
    }
    
    if (myrtx_hash_table_size(table) != 1) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size changed after updating a key");
    }
    
    if (!myrtx_hash_table_get(table, "key1", 0, &out_value, &out_size)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to get updated key1");
    }
    
    if (out_size != sizeof(int) || *(int*)out_value != value2) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Retrieved updated value does not match");
    }
    
    /* Test nonexistent key */
    if (myrtx_hash_table_get(table, "nonexistent", 0, &out_value, &out_size)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Get returned true for nonexistent key");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test removing entries */
void test_remove(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 16, 
                                                      myrtx_hash_string, 
                                                      myrtx_compare_string_keys);
    if (!table) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table");
    }
    
    /* Add some entries */
    int values[3] = {10, 20, 30};
    if (!myrtx_hash_table_put(table, "key1", 0, &values[0], sizeof(int)) ||
        !myrtx_hash_table_put(table, "key2", 0, &values[1], sizeof(int)) ||
        !myrtx_hash_table_put(table, "key3", 0, &values[2], sizeof(int))) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to put keys");
    }
    
    /* Verify size */
    if (myrtx_hash_table_size(table) != 3) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size is not 3 after putting three keys");
    }
    
    /* Test remove */
    if (!myrtx_hash_table_remove(table, "key2", 0, false, false)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to remove key2");
    }
    
    /* Verify size after remove */
    if (myrtx_hash_table_size(table) != 2) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size is not 2 after removing one key");
    }
    
    /* Verify key was removed */
    if (myrtx_hash_table_contains_key(table, "key2", 0)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Key2 still exists after removal");
    }
    
    /* Verify other keys still exist */
    if (!myrtx_hash_table_contains_key(table, "key1", 0) ||
        !myrtx_hash_table_contains_key(table, "key3", 0)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Other keys don't exist after removing key2");
    }
    
    /* Test removing nonexistent key */
    if (myrtx_hash_table_remove(table, "nonexistent", 0, false, false)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Remove returned true for nonexistent key");
    }
    
    /* Verify size did not change */
    if (myrtx_hash_table_size(table) != 2) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size changed after removing nonexistent key");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test clearing the hash table */
void test_clear(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 16, 
                                                      myrtx_hash_string, 
                                                      myrtx_compare_string_keys);
    if (!table) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table");
    }
    
    /* Add some entries */
    int values[3] = {10, 20, 30};
    if (!myrtx_hash_table_put(table, "key1", 0, &values[0], sizeof(int)) ||
        !myrtx_hash_table_put(table, "key2", 0, &values[1], sizeof(int)) ||
        !myrtx_hash_table_put(table, "key3", 0, &values[2], sizeof(int))) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to put keys");
    }
    
    /* Clear the table */
    myrtx_hash_table_clear(table, false, false);
    
    /* Verify size */
    if (myrtx_hash_table_size(table) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size is not 0 after clearing");
    }
    
    /* Verify keys were removed */
    if (myrtx_hash_table_contains_key(table, "key1", 0) ||
        myrtx_hash_table_contains_key(table, "key2", 0) ||
        myrtx_hash_table_contains_key(table, "key3", 0)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Keys still exist after clearing");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test resizing the hash table */
void test_resize(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Start with a small capacity */
    myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 2, 
                                                      myrtx_hash_string, 
                                                      myrtx_compare_string_keys);
    if (!table) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table");
    }
    
    /* Add many entries to trigger resizing */
    char key_buffer[32];
    int value;
    const int num_entries = 100;
    
    for (int i = 0; i < num_entries; i++) {
        sprintf(key_buffer, "key_%d", i);
        value = i;
        
        if (!myrtx_hash_table_put(table, key_buffer, 0, &value, sizeof(int))) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to put key during resize test");
        }
    }
    
    /* Verify all entries are accessible */
    void* out_value;
    size_t out_size;
    
    for (int i = 0; i < num_entries; i++) {
        sprintf(key_buffer, "key_%d", i);
        
        if (!myrtx_hash_table_get(table, key_buffer, 0, &out_value, &out_size)) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to get key after resize");
        }
        
        if (out_size != sizeof(int) || *(int*)out_value != i) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Retrieved value incorrect after resize");
        }
    }
    
    /* Verify size */
    if (myrtx_hash_table_size(table) != num_entries) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Size incorrect after resize");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test hash table with integer keys */
void test_integer_keys(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 16, 
                                                      myrtx_hash_integer, 
                                                      myrtx_compare_integer_keys);
    if (!table) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table for integer keys");
    }
    
    /* Add entries with integer keys */
    int keys[3] = {10, 20, 30};
    const char* values[3] = {"Value 10", "Value 20", "Value 30"};
    
    for (int i = 0; i < 3; i++) {
        if (!myrtx_hash_table_put(table, &keys[i], sizeof(int), 
                                 values[i], strlen(values[i]) + 1)) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to put integer key");
        }
    }
    
    /* Verify entries */
    void* out_value;
    size_t out_size;
    
    for (int i = 0; i < 3; i++) {
        if (!myrtx_hash_table_get(table, &keys[i], sizeof(int), &out_value, &out_size)) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to get integer key");
        }
        
        if (strcmp((const char*)out_value, values[i]) != 0) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Retrieved value incorrect for integer key");
        }
    }
    
    /* Test nonexistent key */
    int nonexistent_key = 50;
    if (myrtx_hash_table_get(table, &nonexistent_key, sizeof(int), &out_value, &out_size)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Get returned true for nonexistent integer key");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test using binary data as keys */
void test_binary_keys(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 16, 
                                                      myrtx_hash_string, 
                                                      myrtx_compare_string_keys);
    if (!table) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create hash table for binary keys");
    }
    
    /* Create some binary keys with null bytes */
    unsigned char binary_key1[] = {0x01, 0x00, 0x02, 0x03, 0x00};
    unsigned char binary_key2[] = {0x01, 0x00, 0x02, 0x04, 0x00};
    
    /* Add entries with binary keys */
    int value1 = 42;
    int value2 = 84;
    
    if (!myrtx_hash_table_put(table, binary_key1, sizeof(binary_key1), 
                             &value1, sizeof(int))) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to put binary key1");
    }
    
    if (!myrtx_hash_table_put(table, binary_key2, sizeof(binary_key2), 
                             &value2, sizeof(int))) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to put binary key2");
    }
    
    /* Verify entries */
    void* out_value;
    size_t out_size;
    
    if (!myrtx_hash_table_get(table, binary_key1, sizeof(binary_key1), 
                             &out_value, &out_size)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to get binary key1");
    }
    
    if (out_size != sizeof(int) || *(int*)out_value != value1) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Retrieved value incorrect for binary key1");
    }
    
    if (!myrtx_hash_table_get(table, binary_key2, sizeof(binary_key2), 
                             &out_value, &out_size)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to get binary key2");
    }
    
    if (out_size != sizeof(int) || *(int*)out_value != value2) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Retrieved value incorrect for binary key2");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test hash table with malloc/free (no arena) */
void test_no_arena(void) {
    /* Create hash table with NULL arena (using malloc) */
    myrtx_hash_table_t* table = myrtx_hash_table_create(NULL, 16, 
                                                      myrtx_hash_string, 
                                                      myrtx_compare_string_keys);
    if (!table) {
        TEST_FAILED("Failed to create hash table with malloc");
    }
    
    /* Add some entries */
    int values[3] = {10, 20, 30};
    if (!myrtx_hash_table_put(table, "key1", 0, &values[0], sizeof(int)) ||
        !myrtx_hash_table_put(table, "key2", 0, &values[1], sizeof(int)) ||
        !myrtx_hash_table_put(table, "key3", 0, &values[2], sizeof(int))) {
        myrtx_hash_table_free(table, true, true);
        TEST_FAILED("Failed to put keys with malloc table");
    }
    
    /* Verify entries */
    void* out_value;
    size_t out_size;
    
    for (int i = 0; i < 3; i++) {
        char key[10];
        sprintf(key, "key%d", i + 1);
        
        if (!myrtx_hash_table_get(table, key, 0, &out_value, &out_size)) {
            myrtx_hash_table_free(table, true, true);
            TEST_FAILED("Failed to get key with malloc table");
        }
        
        if (out_size != sizeof(int) || *(int*)out_value != values[i]) {
            myrtx_hash_table_free(table, true, true);
            TEST_FAILED("Retrieved value incorrect with malloc table");
        }
    }
    
    /* Free the hash table with free_keys and free_values set to true */
    myrtx_hash_table_free(table, true, true);
    
    TEST_PASSED();
}

int main(void) {
    printf("=== myrtx Hash Table Tests ===\n\n");
    
    test_create_free();
    test_basic_operations();
    test_remove();
    test_clear();
    test_resize();
    test_integer_keys();
    test_binary_keys();
    test_no_arena();
    
    printf("\nAll hash table tests successful!\n");
    return 0;
} 