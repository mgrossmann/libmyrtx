/**
 * @file avl_tree.h
 * @brief AVL tree implementation for the myrtx library
 * 
 * This file contains the definitions and functions for a self-balancing
 * AVL tree, optimized for efficient search, insertion, and deletion,
 * providing a guaranteed time complexity of O(log n) for these operations.
 */

#ifndef MYRTX_AVL_TREE_H
#define MYRTX_AVL_TREE_H

#include "myrtx/memory/arena_allocator.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque type for an AVL tree
 */
typedef struct myrtx_avl_tree_t myrtx_avl_tree_t;

/**
 * @brief Opaque type for a node in the AVL tree
 */
typedef struct myrtx_avl_node_t myrtx_avl_node_t;

/**
 * @brief Comparison function for keys in the AVL tree
 * 
 * @param key1 Pointer to the first key
 * @param key2 Pointer to the second key
 * @param user_data User-defined data for comparison
 * @return <0 if key1 < key2, 0 if key1 == key2, >0 if key1 > key2
 */
typedef int (*myrtx_avl_compare_function)(const void* key1, const void* key2, void* user_data);

/**
 * @brief Function to free keys and values
 * 
 * @param key Pointer to the key
 * @param value Pointer to the value
 * @param user_data User-defined data
 */
typedef void (*myrtx_avl_free_function)(void* key, void* value, void* user_data);

/**
 * @brief Visit function for traversing the AVL tree
 * 
 * @param key Pointer to the key of the current node
 * @param value Pointer to the value of the current node
 * @param user_data User-defined data
 * @return true to continue traversal, false to stop
 */
typedef bool (*myrtx_avl_visit_function)(const void* key, void* value, void* user_data);

/**
 * @brief Creates a new AVL tree
 * 
 * @param arena Optional arena allocator (NULL for malloc/free)
 * @param compare_function Comparison function for keys
 * @param user_data User-defined data for the comparison function
 * @return Pointer to the new AVL tree or NULL on error
 */
myrtx_avl_tree_t* myrtx_avl_tree_create(myrtx_arena_t* arena,
                                       myrtx_avl_compare_function compare_function,
                                       void* user_data);

/**
 * @brief Frees an AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @param free_function Optional function to free keys and values (NULL if not needed)
 * @param user_data User-defined data for the free function
 */
void myrtx_avl_tree_free(myrtx_avl_tree_t* tree,
                        myrtx_avl_free_function free_function,
                        void* user_data);

/**
 * @brief Inserts or updates a key-value pair in the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @param key Pointer to the key
 * @param value Pointer to the value
 * @param existing_value Pointer to a location to store the previous value (NULL if not needed)
 * @return true on success, false on error
 */
bool myrtx_avl_tree_insert(myrtx_avl_tree_t* tree,
                          void* key,
                          void* value,
                          void** existing_value);

/**
 * @brief Searches for a value in the AVL tree by key
 * 
 * @param tree Pointer to the AVL tree
 * @param key Pointer to the key
 * @param value_out Pointer to a location for the found value (NULL if not needed)
 * @return true if the key was found, false otherwise
 */
bool myrtx_avl_tree_find(const myrtx_avl_tree_t* tree,
                        const void* key,
                        void** value_out);

/**
 * @brief Removes an entry from the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @param key Pointer to the key
 * @param key_out Pointer to a location for the removed key (NULL if not needed)
 * @param value_out Pointer to a location for the removed value (NULL if not needed)
 * @return true if the key was found and removed, false otherwise
 */
bool myrtx_avl_tree_remove(myrtx_avl_tree_t* tree,
                          const void* key,
                          void** key_out,
                          void** value_out);

/**
 * @brief Returns the number of entries in the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @return Number of entries
 */
size_t myrtx_avl_tree_size(const myrtx_avl_tree_t* tree);

/**
 * @brief Checks if the AVL tree is empty
 * 
 * @param tree Pointer to the AVL tree
 * @return true if the tree is empty, false otherwise
 */
bool myrtx_avl_tree_is_empty(const myrtx_avl_tree_t* tree);

/**
 * @brief Removes all entries from the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @param free_function Optional function to free keys and values (NULL if not needed)
 * @param user_data User-defined data for the free function
 */
void myrtx_avl_tree_clear(myrtx_avl_tree_t* tree,
                         myrtx_avl_free_function free_function,
                         void* user_data);

/**
 * @brief Traverses the AVL tree in-order
 * 
 * @param tree Pointer to the AVL tree
 * @param visit_function Function called for each node
 * @param user_data User-defined data for the visit function
 */
void myrtx_avl_tree_traverse_inorder(const myrtx_avl_tree_t* tree,
                                    myrtx_avl_visit_function visit_function,
                                    void* user_data);

/**
 * @brief Traverses the AVL tree in pre-order
 * 
 * @param tree Pointer to the AVL tree
 * @param visit_function Function called for each node
 * @param user_data User-defined data for the visit function
 */
void myrtx_avl_tree_traverse_preorder(const myrtx_avl_tree_t* tree,
                                     myrtx_avl_visit_function visit_function,
                                     void* user_data);

/**
 * @brief Traverses the AVL tree in post-order
 * 
 * @param tree Pointer to the AVL tree
 * @param visit_function Function called for each node
 * @param user_data User-defined data for the visit function
 */
void myrtx_avl_tree_traverse_postorder(const myrtx_avl_tree_t* tree,
                                      myrtx_avl_visit_function visit_function,
                                      void* user_data);

/**
 * @brief Finds the smallest key in the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @param key_out Pointer to a location for the smallest key (NULL if not needed)
 * @param value_out Pointer to a location for the associated value (NULL if not needed)
 * @return true if the tree is not empty, false otherwise
 */
bool myrtx_avl_tree_min(const myrtx_avl_tree_t* tree,
                       void** key_out,
                       void** value_out);

/**
 * @brief Finds the largest key in the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @param key_out Pointer to a location for the largest key (NULL if not needed)
 * @param value_out Pointer to a location for the associated value (NULL if not needed)
 * @return true if the tree is not empty, false otherwise
 */
bool myrtx_avl_tree_max(const myrtx_avl_tree_t* tree,
                       void** key_out,
                       void** value_out);

/**
 * @brief Returns the height of the AVL tree
 * 
 * @param tree Pointer to the AVL tree
 * @return Height of the tree (0 for empty tree)
 */
size_t myrtx_avl_tree_height(const myrtx_avl_tree_t* tree);

/**
 * @brief Checks if the AVL tree contains a specific key
 * 
 * @param tree Pointer to the AVL tree
 * @param key Pointer to the key to search for
 * @return true if the key was found, false otherwise
 */
bool myrtx_avl_tree_contains(const myrtx_avl_tree_t* tree, const void* key);

/**
 * @brief Standard comparison function for string keys
 * 
 * @param key1 Pointer to the first string
 * @param key2 Pointer to the second string
 * @param user_data Not used
 * @return int Comparison result as from strcmp
 */
int myrtx_avl_compare_strings(const void* key1, const void* key2, void* user_data);

/**
 * @brief Standard comparison function for integer keys
 * 
 * @param key1 Pointer to the first integer
 * @param key2 Pointer to the second integer
 * @param user_data Not used
 * @return int Comparison result (<0, 0, >0)
 */
int myrtx_avl_compare_integers(const void* key1, const void* key2, void* user_data);

#ifdef __cplusplus
}
#endif

#endif /* MYRTX_AVL_TREE_H */ 