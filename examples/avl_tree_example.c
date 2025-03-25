/**
 * @file avl_tree_example.c
 * @brief Example for using the AVL tree
 */

#include "myrtx/collections/avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Example for string keys */
void string_keys_example(void) {
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    void* value;
    
    printf("=== String Keys Example ===\n");
    
    /* Initialize arena */
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Error initializing arena\n");
        return;
    }
    
    /* Create tree with string keys */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    if (!tree) {
        printf("Error creating AVL tree\n");
        myrtx_arena_free(&arena);
        return;
    }
    
    /* Insert key-value pairs */
    printf("Inserting key-value pairs...\n");
    
    int value1 = 42;
    if (myrtx_avl_tree_insert(tree, "answer", &value1, NULL)) {
        printf("Key 'answer' inserted\n");
    }
    
    double value2 = 3.14159;
    if (myrtx_avl_tree_insert(tree, "pi", &value2, NULL)) {
        printf("Key 'pi' inserted\n");
    }
    
    const char* value3 = "Hello, World!";
    if (myrtx_avl_tree_insert(tree, "greeting", (void*)value3, NULL)) {
        printf("Key 'greeting' inserted\n");
    }
    
    /* Query the tree */
    printf("\nQuerying the AVL tree:\n");
    
    if (myrtx_avl_tree_contains(tree, "answer")) {
        printf("Key 'answer' exists\n");
        
        if (myrtx_avl_tree_find(tree, "answer", &value)) {
            printf("Value: %d\n", *(int*)value);
        }
    }
    
    if (myrtx_avl_tree_contains(tree, "pi")) {
        printf("Key 'pi' exists\n");
        
        if (myrtx_avl_tree_find(tree, "pi", &value)) {
            printf("Value: %f\n", *(double*)value);
        }
    }
    
    if (myrtx_avl_tree_contains(tree, "greeting")) {
        printf("Key 'greeting' exists\n");
        
        if (myrtx_avl_tree_find(tree, "greeting", &value)) {
            printf("Value: %s\n", (char*)value);
        }
    }
    
    if (!myrtx_avl_tree_contains(tree, "nonexistent")) {
        printf("Key 'nonexistent' does not exist\n");
    }
    
    /* Update value */
    printf("\nUpdating value...\n");
    
    int new_value = 100;
    void* old_value = NULL;
    if (myrtx_avl_tree_insert(tree, "answer", &new_value, &old_value)) {
        printf("Key 'answer' updated\n");
        printf("Old value: %d\n", *(int*)old_value);
        
        if (myrtx_avl_tree_find(tree, "answer", &value)) {
            printf("New value: %d\n", *(int*)value);
        }
    }
    
    /* Traversal (In-Order) */
    printf("\nTraversal (In-Order):\n");
    printf("This function is demonstrated in visit_node_callback.\n");
    
    /* Remove key */
    printf("\nRemoving key...\n");
    
    void* removed_key = NULL;
    void* removed_value = NULL;
    if (myrtx_avl_tree_remove(tree, "pi", &removed_key, &removed_value)) {
        printf("Key '%s' removed\n", (char*)removed_key);
        printf("Removed value: %f\n", *(double*)removed_value);
    }
    
    if (!myrtx_avl_tree_contains(tree, "pi")) {
        printf("Key 'pi' no longer exists\n");
    }
    
    /* Find min and max */
    printf("\nFinding min and max...\n");
    
    void* min_key = NULL;
    void* min_value = NULL;
    if (myrtx_avl_tree_min(tree, &min_key, &min_value)) {
        printf("Minimum key: %s\n", (char*)min_key);
    }
    
    void* max_key = NULL;
    void* max_value = NULL;
    if (myrtx_avl_tree_max(tree, &max_key, &max_value)) {
        printf("Maximum key: %s\n", (char*)max_key);
    }
    
    /* Current size and height */
    printf("\nCurrent tree size: %zu\n", myrtx_avl_tree_size(tree));
    printf("Current tree height: %zu\n", myrtx_avl_tree_height(tree));
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);  /* No need to free when using arena */
    myrtx_arena_free(&arena);
    
    printf("String keys example completed\n\n");
}

/* Example for integer keys */
void integer_keys_example(void) {
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    void* value;
    
    printf("=== Integer Keys Example ===\n");
    
    /* Initialize arena */
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Error initializing arena\n");
        return;
    }
    
    /* Create tree with integer keys */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_integers, NULL);
    if (!tree) {
        printf("Error creating AVL tree\n");
        myrtx_arena_free(&arena);
        return;
    }
    
    /* Insert key-value pairs */
    printf("Inserting key-value pairs with integer keys...\n");
    
    int keys[5] = {50, 30, 70, 20, 40};
    const char* values[5] = {"Fifty", "Thirty", "Seventy", "Twenty", "Forty"};
    
    for (int i = 0; i < 5; i++) {
        if (myrtx_avl_tree_insert(tree, &keys[i], (void*)values[i], NULL)) {
            printf("Key %d inserted\n", keys[i]);
        }
    }
    
    /* Query the tree */
    printf("\nQuerying the AVL tree:\n");
    
    for (int i = 0; i < 5; i++) {
        if (myrtx_avl_tree_contains(tree, &keys[i])) {
            printf("Key %d exists\n", keys[i]);
            
            if (myrtx_avl_tree_find(tree, &keys[i], &value)) {
                printf("Value: %s\n", (char*)value);
            }
        }
    }
    
    /* Find min and max */
    printf("\nFinding min and max...\n");
    
    void* min_key = NULL;
    void* min_value = NULL;
    if (myrtx_avl_tree_min(tree, &min_key, &min_value)) {
        printf("Minimum key: %d\n", *(int*)min_key);
        printf("Associated value: %s\n", (char*)min_value);
    }
    
    void* max_key = NULL;
    void* max_value = NULL;
    if (myrtx_avl_tree_max(tree, &max_key, &max_value)) {
        printf("Maximum key: %d\n", *(int*)max_key);
        printf("Associated value: %s\n", (char*)max_value);
    }
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);  /* No need to free when using arena */
    myrtx_arena_free(&arena);
    
    printf("Integer keys example completed\n\n");
}

/* Callback function for traversal */
bool visit_node_callback(const void* key, void* value, void* user_data) {
    /* Cast key to required type */
    const char* key_str = (const char*)key;
    
    /* Verwendung von value um unused-Parameter-Warnung zu vermeiden */
    (void)value;
    
    /* Get user data */
    int* counter = (int*)user_data;
    
    /* Print info */
    printf("  Node: %s", key_str);
    
    /* Update counter */
    (*counter)++;
    
    /* Continue traversal */
    return true;
}

/* Example for traversal */
void traversal_example(void) {
    myrtx_arena_t arena = {0};
    myrtx_avl_tree_t* tree;
    
    printf("=== Traversal Example ===\n");
    
    /* Initialize arena */
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Error initializing arena\n");
        return;
    }
    
    /* Create tree with string keys */
    tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    if (!tree) {
        printf("Error creating AVL tree\n");
        myrtx_arena_free(&arena);
        return;
    }
    
    /* Insert key-value pairs */
    const char* keys[] = {"m", "e", "r", "a", "h", "p", "z"};
    int values[] = {1, 2, 3, 4, 5, 6, 7};
    int num_keys = sizeof(keys) / sizeof(keys[0]);
    
    printf("Inserting %d keys...\n", num_keys);
    for (int i = 0; i < num_keys; i++) {
        if (myrtx_avl_tree_insert(tree, (void*)keys[i], &values[i], NULL)) {
            printf("Key '%s' inserted\n", keys[i]);
        }
    }
    
    /* Perform various traversals */
    printf("\nIn-Order Traversal (should be alphabetically sorted):\n");
    myrtx_avl_tree_traverse_inorder(tree, visit_node_callback, (void*)"z");
    
    printf("\nPre-Order Traversal:\n");
    myrtx_avl_tree_traverse_preorder(tree, visit_node_callback, (void*)"h");
    
    printf("\nPost-Order Traversal:\n");
    myrtx_avl_tree_traverse_postorder(tree, visit_node_callback, (void*)"a");
    
    /* Cleanup */
    myrtx_avl_tree_free(tree, NULL, NULL);  /* No need to free when using arena */
    myrtx_arena_free(&arena);
    
    printf("Traversal example completed\n\n");
}

/* Free function for keys and values */
void key_value_free_func(void* key, void* value, void* user_data) {
    int* counter = (int*)user_data;
    if (counter) {
        (*counter)++;
    }
    free(key);
    free(value);
}

/* Example for usage without arena (with malloc/free) */
void without_arena_example(void) {
    myrtx_avl_tree_t* tree;
    
    printf("=== Example Without Arena ===\n");
    
    /* Create tree without arena (uses malloc/free) */
    tree = myrtx_avl_tree_create(NULL, myrtx_avl_compare_strings, NULL);
    if (!tree) {
        printf("Error creating AVL tree\n");
        return;
    }
    
    /* Strings to insert (dynamically allocated) */
    char* key1 = strdup("key1");
    char* key2 = strdup("key2");
    char* value1 = strdup("value1");
    char* value2 = strdup("value2");
    
    /* Insert key-value pairs */
    printf("Inserting dynamically allocated keys and values...\n");
    
    if (myrtx_avl_tree_insert(tree, key1, value1, NULL)) {
        printf("Key '%s' inserted\n", key1);
    }
    
    if (myrtx_avl_tree_insert(tree, key2, value2, NULL)) {
        printf("Key '%s' inserted\n", key2);
    }
    
    /* Counter for releases */
    int free_counter = 0;
    
    /* Free the tree and all keys and values */
    printf("\nFreeing tree and all keys/values...\n");
    myrtx_avl_tree_free(tree, key_value_free_func, &free_counter);
    
    printf("Number of freed entries: %d\n", free_counter);
    printf("Example without arena completed\n\n");
}

int main(void) {
    printf("=== AVL Tree Example ===\n\n");
    
    string_keys_example();
    integer_keys_example();
    traversal_example();
    without_arena_example();
    
    printf("All examples completed successfully\n");
    return 0;
} 