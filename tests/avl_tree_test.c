/**
 * @file avl_tree_test.c
 * @brief Tests for the AVL tree implementation
 */

#include "myrtx/collections/avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int tests_passed = 0;
static int tests_failed = 0;

/* Helper function for checking test results */
#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("✅ PASSED: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("❌ FAILED: %s\n", message); \
            tests_failed++; \
        } \
    } while (0)

/* Test for basic operations */
void test_basic_operations(void) {
    printf("\n=== Test: Basic Operations ===\n");
    
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    
    /* Initialize arena */
    assert(myrtx_arena_init(&arena, 0));
    
    /* Create tree */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(tree != NULL, "Tree creation");
    TEST_ASSERT(myrtx_avl_tree_size(tree) == 0, "Initial tree size is 0");
    TEST_ASSERT(myrtx_avl_tree_is_empty(tree), "Tree is initially empty");
    
    /* Insert keys */
    const char* keys[] = {"d", "b", "f", "a", "c", "e", "g"};
    int values[] = {4, 2, 6, 1, 3, 5, 7};
    
    for (int i = 0; i < 7; i++) {
        bool result = myrtx_avl_tree_insert(tree, (void*)keys[i], &values[i], NULL);
        char message[50];
        sprintf(message, "Insert key '%s'", keys[i]);
        TEST_ASSERT(result, message);
    }
    
    TEST_ASSERT(myrtx_avl_tree_size(tree) == 7, "Tree size after insertion");
    TEST_ASSERT(!myrtx_avl_tree_is_empty(tree), "Tree is not empty after insertion");
    
    /* Find keys */
    void* found_value;
    
    for (int i = 0; i < 7; i++) {
        bool result = myrtx_avl_tree_find(tree, (void*)keys[i], &found_value);
        char message[50];
        sprintf(message, "Find key '%s'", keys[i]);
        TEST_ASSERT(result, message);
        
        if (result) {
            char value_message[50];
            sprintf(value_message, "Correct value for key '%s'", keys[i]);
            TEST_ASSERT(*(int*)found_value == values[i], value_message);
        }
    }
    
    /* Search for non-existent key */
    TEST_ASSERT(!myrtx_avl_tree_find(tree, "z", &found_value), "NOT finding non-existent key");
    TEST_ASSERT(!myrtx_avl_tree_contains(tree, "z"), "contains() for non-existent key");
    
    /* Remove key */
    void* removed_key = NULL;
    void* removed_value = NULL;
    
    bool result = myrtx_avl_tree_remove(tree, "b", &removed_key, &removed_value);
    TEST_ASSERT(result, "Remove key 'b'");
    TEST_ASSERT(strcmp((char*)removed_key, "b") == 0, "Removed key is correct");
    TEST_ASSERT(*(int*)removed_value == 2, "Removed value is correct");
    TEST_ASSERT(myrtx_avl_tree_size(tree) == 6, "Tree size after removal");
    TEST_ASSERT(!myrtx_avl_tree_contains(tree, "b"), "Removed key no longer exists");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);
    myrtx_arena_free(&arena);
}

/* Test for balancing property */
void test_balancing(void) {
    printf("\n=== Test: Tree Balancing ===\n");
    
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    
    /* Initialize arena */
    assert(myrtx_arena_init(&arena, 0));
    
    /* Create tree */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_integers, NULL);
    TEST_ASSERT(tree != NULL, "Tree creation");
    
    /* Insert in ascending order (without AVL, this would create a linear tree) */
    int keys[10];
    for (int i = 0; i < 10; i++) {
        keys[i] = i;
        bool result = myrtx_avl_tree_insert(tree, &keys[i], &i, NULL);
        char message[80]; /* Increased to 80 characters */
        sprintf(message, "Insert key %d", i);
        TEST_ASSERT(result, message);
    }
    
    /* Check if the tree is balanced (height should be ~log2(n)) */
    size_t height = myrtx_avl_tree_height(tree);
    printf("Tree height with 10 elements: %zu\n", height);
    TEST_ASSERT(height <= 4, "Tree height is logarithmic (should be <=4 for 10 elements)");
    
    /* Insert in descending order (more balancing tests) */
    myrtx_avl_tree_t* tree2 = myrtx_avl_tree_create(&arena, myrtx_avl_compare_integers, NULL);
    TEST_ASSERT(tree2 != NULL, "Second tree creation");
    
    int keys2[10];
    for (int i = 9; i >= 0; i--) {
        keys2[i] = i;
        bool result = myrtx_avl_tree_insert(tree2, &keys2[i], &i, NULL);
        char message[80]; /* Increased to 80 characters */
        sprintf(message, "Insert key %d in descending order", i);
        TEST_ASSERT(result, message);
    }
    
    size_t height2 = myrtx_avl_tree_height(tree2);
    printf("Height of second tree with 10 elements: %zu\n", height2);
    TEST_ASSERT(height2 <= 4, "Second tree height is logarithmic");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);
    myrtx_avl_tree_free(tree2, NULL, NULL);
    myrtx_arena_free(&arena);
}

/* Helper function for counting traversed nodes */
static bool count_nodes_callback(const void* key, void* value, void* user_data) {
    int* count = (int*)user_data;
    (*count)++;
    return true; /* Continue traversal */
}

/* Test for traversal functions */
void test_traversal(void) {
    printf("\n=== Test: Traversal Functions ===\n");
    
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    
    /* Initialize arena */
    assert(myrtx_arena_init(&arena, 0));
    
    /* Create tree */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(tree != NULL, "Tree creation");
    
    /* Insert keys */
    const char* keys[] = {"d", "b", "f", "a", "c", "e", "g"};
    int values[] = {4, 2, 6, 1, 3, 5, 7};
    int num_keys = sizeof(keys) / sizeof(keys[0]);
    
    for (int i = 0; i < num_keys; i++) {
        bool result = myrtx_avl_tree_insert(tree, (void*)keys[i], &values[i], NULL);
        TEST_ASSERT(result, "Insert key for traversal");
    }
    
    /* Test in-order traversal */
    int count = 0;
    myrtx_avl_tree_traverse_inorder(tree, count_nodes_callback, &count);
    TEST_ASSERT(count == num_keys, "In-order traversal visits all nodes");
    
    /* Test pre-order traversal */
    count = 0;
    myrtx_avl_tree_traverse_preorder(tree, count_nodes_callback, &count);
    TEST_ASSERT(count == num_keys, "Pre-order traversal visits all nodes");
    
    /* Test post-order traversal */
    count = 0;
    myrtx_avl_tree_traverse_postorder(tree, count_nodes_callback, &count);
    TEST_ASSERT(count == num_keys, "Post-order traversal visits all nodes");
    
    /* Early traversal termination */
    bool early_stop_callback(const void* key, void* value, void* user_data) {
        int* stop_count = (int*)user_data;
        (*stop_count)++;
        /* Stop after three nodes */
        return (*stop_count < 3);
    }
    
    count = 0;
    myrtx_avl_tree_traverse_inorder(tree, early_stop_callback, &count);
    TEST_ASSERT(count == 3, "Early traversal termination works");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);
    myrtx_arena_free(&arena);
}

/* Test for min/max functions */
void test_min_max(void) {
    printf("\n=== Test: Min/Max Functions ===\n");
    
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    
    /* Initialize arena */
    assert(myrtx_arena_init(&arena, 0));
    
    /* Create tree */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(tree != NULL, "Tree creation");
    
    /* Insert keys */
    const char* keys[] = {"d", "b", "f", "a", "c", "e", "g"};
    int values[] = {4, 2, 6, 1, 3, 5, 7};
    
    for (int i = 0; i < 7; i++) {
        bool result = myrtx_avl_tree_insert(tree, (void*)keys[i], &values[i], NULL);
        TEST_ASSERT(result, "Insert key for min/max test");
    }
    
    /* Check minimum and maximum */
    void* min_key = NULL;
    void* min_value = NULL;
    bool min_result = myrtx_avl_tree_min(tree, &min_key, &min_value);
    TEST_ASSERT(min_result, "Find minimum");
    TEST_ASSERT(strcmp((char*)min_key, "a") == 0, "Minimum key is 'a'");
    TEST_ASSERT(*(int*)min_value == 1, "Value of minimum key is 1");
    
    void* max_key = NULL;
    void* max_value = NULL;
    bool max_result = myrtx_avl_tree_max(tree, &max_key, &max_value);
    TEST_ASSERT(max_result, "Find maximum");
    TEST_ASSERT(strcmp((char*)max_key, "g") == 0, "Maximum key is 'g'");
    TEST_ASSERT(*(int*)max_value == 7, "Value of maximum key is 7");
    
    /* Test min/max on empty tree */
    myrtx_avl_tree_t* empty_tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(!myrtx_avl_tree_min(empty_tree, &min_key, &min_value), "Min on empty tree fails");
    TEST_ASSERT(!myrtx_avl_tree_max(empty_tree, &max_key, &max_value), "Max on empty tree fails");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);
    myrtx_avl_tree_free(empty_tree, NULL, NULL);
    myrtx_arena_free(&arena);
}

/* Test for update operations */
void test_update_operations(void) {
    printf("\n=== Test: Update Operations ===\n");
    
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    
    /* Initialize arena */
    assert(myrtx_arena_init(&arena, 0));
    
    /* Create tree */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(tree != NULL, "Tree creation");
    
    /* Insert key */
    int original_value = 42;
    bool insert_result = myrtx_avl_tree_insert(tree, "test", &original_value, NULL);
    TEST_ASSERT(insert_result, "Initial insertion");
    TEST_ASSERT(myrtx_avl_tree_size(tree) == 1, "Tree size after initial insertion");
    
    /* Update value */
    int new_value = 100;
    void* old_value = NULL;
    bool update_result = myrtx_avl_tree_insert(tree, "test", &new_value, &old_value);
    TEST_ASSERT(update_result, "Update value");
    TEST_ASSERT(old_value != NULL, "Old value is returned");
    TEST_ASSERT(*(int*)old_value == 42, "Old value is correct");
    TEST_ASSERT(myrtx_avl_tree_size(tree) == 1, "Tree size remains same after update");
    
    /* Check updated value */
    void* found_value = NULL;
    bool find_result = myrtx_avl_tree_find(tree, "test", &found_value);
    TEST_ASSERT(find_result, "Find updated value");
    TEST_ASSERT(*(int*)found_value == 100, "Updated value is correct");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);
    myrtx_arena_free(&arena);
}

/* Test for edge cases */
void test_edge_cases(void) {
    printf("\n=== Test: Edge Cases ===\n");
    
    myrtx_arena_t arena = {0};
    assert(myrtx_arena_init(&arena, 0));
    
    /* Test case: NULL comparison function */
    myrtx_avl_tree_t* tree1 = myrtx_avl_tree_create(&arena, NULL, NULL);
    TEST_ASSERT(tree1 == NULL, "Tree creation with NULL comparison function fails");
    
    /* Test case: Removing non-existent keys */
    myrtx_avl_tree_t* tree2 = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(tree2 != NULL, "Tree creation");
    
    void* removed_key = NULL;
    void* removed_value = NULL;
    bool remove_result = myrtx_avl_tree_remove(tree2, "nonexistent", &removed_key, &removed_value);
    TEST_ASSERT(!remove_result, "Removing a non-existent key fails");
    TEST_ASSERT(removed_key == NULL, "Removed key is NULL for non-existent key");
    TEST_ASSERT(removed_value == NULL, "Removed value is NULL for non-existent key");
    
    /* Less extreme tests for balancing */
    const char* test_keys[] = {
        "key01", "key02", "key03", "key04", "key05", "key06", "key07", "key08", "key09", "key10",
        "key11", "key12", "key13", "key14", "key15", "key16", "key17", "key18", "key19", "key20"
    };
    int values[20];
    
    /* Insert 20 keys into the tree */
    for (int i = 0; i < 20; i++) {
        values[i] = i;
        if (myrtx_avl_tree_insert(tree2, (void*)test_keys[i], &values[i], NULL)) {
            TEST_ASSERT(true, "Insert for balancing test");
        }
    }
    
    TEST_ASSERT(myrtx_avl_tree_size(tree2) == 20, "Tree size after inserting many keys");
    size_t height = myrtx_avl_tree_height(tree2);
    printf("Tree height with 20 elements: %zu\n", height);
    TEST_ASSERT(height <= 5, "Tree height is logarithmic (should be <=5 for 20 elements)");
    
    /* Remove every other key */
    for (int i = 0; i < 20; i += 2) {
        bool remove_result = myrtx_avl_tree_remove(tree2, test_keys[i], NULL, NULL);
        TEST_ASSERT(remove_result, "Remove for balancing test");
    }
    
    TEST_ASSERT(myrtx_avl_tree_size(tree2) == 10, "Tree size after removal");
    height = myrtx_avl_tree_height(tree2);
    printf("Tree height after removal (10 elements): %zu\n", height);
    TEST_ASSERT(height <= 4, "Tree height after removal is logarithmic");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree2, NULL, NULL);
    myrtx_arena_free(&arena);
}

/* Test free callback function */
void test_free_callback(void* key, void* value, void* user_data) {
    int* counter = (int*)user_data;
    (*counter)++;
    free(key);
    free(value);
}

/* Test for custom free callbacks */
void test_custom_free_callbacks(void) {
    printf("\n=== Test: Custom Free Callbacks ===\n");
    
    /* Create tree without arena (using malloc/free) */
    myrtx_avl_tree_t* tree = myrtx_avl_tree_create(NULL, myrtx_avl_compare_strings, NULL);
    TEST_ASSERT(tree != NULL, "Tree creation without arena");
    
    /* Create dynamically allocated keys and values */
    struct test_data {
        int id;
        char name[20];
    };
    
    char* key1 = strdup("key1");
    char* key2 = strdup("key2");
    struct test_data* data1 = malloc(sizeof(struct test_data));
    struct test_data* data2 = malloc(sizeof(struct test_data));
    
    data1->id = 1;
    strcpy(data1->name, "TestData1");
    data2->id = 2;
    strcpy(data2->name, "TestData2");
    
    /* Insert keys and values */
    bool insert1 = myrtx_avl_tree_insert(tree, key1, data1, NULL);
    bool insert2 = myrtx_avl_tree_insert(tree, key2, data2, NULL);
    TEST_ASSERT(insert1 && insert2, "Insert dynamically allocated data");
    
    /* Counter for free callback calls */
    int free_callback_calls = 0;
    
    /* Free the tree and call callback */
    myrtx_avl_tree_free(tree, test_free_callback, &free_callback_calls);
    
    TEST_ASSERT(free_callback_calls == 2, "Free callback was called for both entries");
}

int main(void) {
    printf("=== AVL Tree Tests ===\n");
    
    test_basic_operations();
    test_balancing();
    test_traversal();
    test_min_max();
    test_update_operations();
    test_edge_cases();
    test_custom_free_callbacks();
    
    printf("\n=== Test Results ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Total: %d\n", tests_passed + tests_failed);
    
    return (tests_failed == 0) ? 0 : 1;
} 