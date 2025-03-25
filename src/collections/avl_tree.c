#include "myrtx/collections/avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * Node structure for the AVL tree
 */
struct myrtx_avl_node_t {
    void* key;                    /* Key */
    void* value;                  /* Value */
    struct myrtx_avl_node_t* left;  /* Left child */
    struct myrtx_avl_node_t* right; /* Right child */
    int height;                   /* Height of the node for AVL balancing */
};

/**
 * AVL tree structure
 */
struct myrtx_avl_tree_t {
    myrtx_avl_node_t* root;       /* Root node */
    size_t size;                  /* Number of entries in the tree */
    myrtx_arena_t* arena;         /* Arena allocator or NULL */
    myrtx_avl_compare_function compare_func; /* Comparison function for keys */
    void* user_data;              /* User-defined data for comparison function */
};

/* Private helper functions */

/**
 * Allocates memory via Arena or malloc
 */
static void* avl_tree_malloc(myrtx_avl_tree_t* tree, size_t size) {
    if (tree->arena) {
        return myrtx_arena_alloc(tree->arena, size);
    } else {
        return malloc(size);
    }
}

/**
 * Creates a new node
 */
static myrtx_avl_node_t* create_node(myrtx_avl_tree_t* tree, void* key, void* value) {
    myrtx_avl_node_t* node = (myrtx_avl_node_t*)avl_tree_malloc(tree, sizeof(myrtx_avl_node_t));
    if (!node) {
        return NULL;
    }
    
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; /* New node has height 1 */
    
    return node;
}

/**
 * Returns the height of a node (0 for NULL)
 */
static int node_height(const myrtx_avl_node_t* node) {
    return node ? node->height : 0;
}

/**
 * Calculates the balance factor of a node
 */
static int balance_factor(const myrtx_avl_node_t* node) {
    if (!node) {
        return 0;
    }
    return node_height(node->left) - node_height(node->right);
}

/**
 * Updates the height of a node based on its children
 */
static void update_height(myrtx_avl_node_t* node) {
    if (!node) {
        return;
    }
    int left_height = node_height(node->left);
    int right_height = node_height(node->right);
    node->height = 1 + (left_height > right_height ? left_height : right_height);
}

/**
 * Right rotation around a node
 */
static myrtx_avl_node_t* rotate_right(myrtx_avl_node_t* y) {
    myrtx_avl_node_t* x = y->left;
    myrtx_avl_node_t* T2 = x->right;
    
    /* Perform rotation */
    x->right = y;
    y->left = T2;
    
    /* Update height */
    update_height(y);
    update_height(x);
    
    return x;
}

/**
 * Left rotation around a node
 */
static myrtx_avl_node_t* rotate_left(myrtx_avl_node_t* x) {
    myrtx_avl_node_t* y = x->right;
    myrtx_avl_node_t* T2 = y->left;
    
    /* Perform rotation */
    y->left = x;
    x->right = T2;
    
    /* Update height */
    update_height(x);
    update_height(y);
    
    return y;
}

/**
 * Balances a node and performs necessary rotations
 */
static myrtx_avl_node_t* balance_node(myrtx_avl_node_t* node) {
    if (!node) {
        return NULL;
    }
    
    /* Update height */
    update_height(node);
    
    /* Calculate balance factor */
    int balance = balance_factor(node);
    
    /* LL case: Right rotation */
    if (balance > 1 && balance_factor(node->left) >= 0) {
        return rotate_right(node);
    }
    
    /* LR case: Left-Right rotation */
    if (balance > 1 && balance_factor(node->left) < 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    
    /* RR case: Left rotation */
    if (balance < -1 && balance_factor(node->right) <= 0) {
        return rotate_left(node);
    }
    
    /* RL case: Right-Left rotation */
    if (balance < -1 && balance_factor(node->right) > 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    
    /* No balancing needed */
    return node;
}

/**
 * Finds the node with the minimum key in the subtree
 */
static myrtx_avl_node_t* find_min_node(myrtx_avl_node_t* node) {
    if (!node) {
        return NULL;
    }
    
    /* Go as far left as possible */
    while (node->left) {
        node = node->left;
    }
    
    return node;
}

/**
 * Recursive insertion of a key-value pair
 */
static myrtx_avl_node_t* insert_node(myrtx_avl_tree_t* tree, 
                                   myrtx_avl_node_t* node, 
                                   void* key, void* value, 
                                   void** existing_value,
                                   bool* is_new) {
    /* Case 1: Empty subtree - create new node */
    if (!node) {
        *is_new = true;
        return create_node(tree, key, value);
    }
    
    /* Compare keys */
    int cmp = tree->compare_func(key, node->key, tree->user_data);
    
    /* Case 2: Key is smaller - insert in left subtree */
    if (cmp < 0) {
        node->left = insert_node(tree, node->left, key, value, existing_value, is_new);
    }
    /* Case 3: Key is larger - insert in right subtree */
    else if (cmp > 0) {
        node->right = insert_node(tree, node->right, key, value, existing_value, is_new);
    }
    /* Case 4: Key already exists - update value */
    else {
        if (existing_value) {
            *existing_value = node->value;
        }
        node->value = value;
        *is_new = false;
    }
    
    /* Balance node */
    return balance_node(node);
}

/**
 * Recursive removal of a node
 */
static myrtx_avl_node_t* remove_node(myrtx_avl_tree_t* tree, 
                                   myrtx_avl_node_t* node, 
                                   const void* key,
                                   void** key_out, void** value_out,
                                   bool* found) {
    if (!node) {
        *found = false;
        return NULL;
    }
    
    /* Compare keys */
    int cmp = tree->compare_func(key, node->key, tree->user_data);
    
    /* Recursively search in the corresponding subtree */
    if (cmp < 0) {
        node->left = remove_node(tree, node->left, key, key_out, value_out, found);
    }
    else if (cmp > 0) {
        node->right = remove_node(tree, node->right, key, key_out, value_out, found);
    }
    /* Key found - remove node */
    else {
        *found = true;
        
        /* Save key and value if desired */
        if (key_out) {
            *key_out = node->key;
        }
        if (value_out) {
            *value_out = node->value;
        }
        
        /* Case 1: Node has fewer than two children */
        if (!node->left || !node->right) {
            myrtx_avl_node_t* temp = node->left ? node->left : node->right;
            
            /* Case 1a: Node has no children */
            if (!temp) {
                temp = node;
                node = NULL;
            }
            /* Case 1b: Node has one child */
            else {
                /* Replace node with its child */
                *node = *temp;
            }
            
            /* Free old node */
            if (!tree->arena) {
                free(temp);
            }
        }
        /* Case 2: Node has two children */
        else {
            /* Find successor (smallest node in right subtree) */
            myrtx_avl_node_t* successor = find_min_node(node->right);
            
            /* Copy key and value of successor */
            node->key = successor->key;
            node->value = successor->value;
            
            /* Recursively remove successor */
            node->right = remove_node(tree, node->right, successor->key, NULL, NULL, found);
        }
    }
    
    /* If tree is empty after removal */
    if (!node) {
        return NULL;
    }
    
    /* Balance node */
    return balance_node(node);
}

/**
 * Recursive search for a node
 */
static myrtx_avl_node_t* find_node(const myrtx_avl_tree_t* tree, 
                                 const myrtx_avl_node_t* node, 
                                 const void* key) {
    if (!node) {
        return NULL;
    }
    
    /* Compare keys */
    int cmp = tree->compare_func(key, node->key, tree->user_data);
    
    if (cmp < 0) {
        return find_node(tree, node->left, key);
    }
    else if (cmp > 0) {
        return find_node(tree, node->right, key);
    }
    else {
        /* Key found - return node */
        return (myrtx_avl_node_t*)node;
    }
}

/**
 * Recursive freeing of the tree
 */
static void free_subtree(myrtx_avl_tree_t* tree, 
                        myrtx_avl_node_t* node,
                        myrtx_avl_free_function free_function,
                        void* user_data) {
    if (!node) {
        return;
    }
    
    /* Recursively free left and right subtree */
    free_subtree(tree, node->left, free_function, user_data);
    free_subtree(tree, node->right, free_function, user_data);
    
    /* If a freeing function is specified, call it */
    if (free_function) {
        free_function(node->key, node->value, user_data);
    }
    
    /* Free node if no arena allocator is used */
    if (!tree->arena) {
        free(node);
    }
}

/**
 * Recursive in-order traversal
 */
static void traverse_inorder_recursive(const myrtx_avl_node_t* node,
                                     myrtx_avl_visit_function visit_function,
                                     void* user_data,
                                     bool* continue_traversal) {
    if (!node || !(*continue_traversal)) {
        return;
    }
    
    /* Traverse left subtree */
    traverse_inorder_recursive(node->left, visit_function, user_data, continue_traversal);
    
    /* Visit current node */
    if (*continue_traversal) {
        *continue_traversal = visit_function(node->key, node->value, user_data);
    }
    
    /* Traverse right subtree */
    if (*continue_traversal) {
        traverse_inorder_recursive(node->right, visit_function, user_data, continue_traversal);
    }
}

/**
 * Recursive pre-order traversal
 */
static void traverse_preorder_recursive(const myrtx_avl_node_t* node,
                                      myrtx_avl_visit_function visit_function,
                                      void* user_data,
                                      bool* continue_traversal) {
    if (!node || !(*continue_traversal)) {
        return;
    }
    
    /* Visit current node */
    *continue_traversal = visit_function(node->key, node->value, user_data);
    
    /* Traverse left and right subtrees */
    if (*continue_traversal) {
        traverse_preorder_recursive(node->left, visit_function, user_data, continue_traversal);
    }
    
    if (*continue_traversal) {
        traverse_preorder_recursive(node->right, visit_function, user_data, continue_traversal);
    }
}

/**
 * Recursive post-order traversal
 */
static void traverse_postorder_recursive(const myrtx_avl_node_t* node,
                                       myrtx_avl_visit_function visit_function,
                                       void* user_data,
                                       bool* continue_traversal) {
    if (!node || !(*continue_traversal)) {
        return;
    }
    
    /* Traverse left and right subtrees */
    traverse_postorder_recursive(node->left, visit_function, user_data, continue_traversal);
    
    if (*continue_traversal) {
        traverse_postorder_recursive(node->right, visit_function, user_data, continue_traversal);
    }
    
    /* Visit current node */
    if (*continue_traversal) {
        *continue_traversal = visit_function(node->key, node->value, user_data);
    }
}

/* Public API implementation */

myrtx_avl_tree_t* myrtx_avl_tree_create(myrtx_arena_t* arena,
                                       myrtx_avl_compare_function compare_function,
                                       void* user_data) {
    /* Parameter check */
    if (!compare_function) {
        return NULL;
    }
    
    /* Allocate memory for tree */
    myrtx_avl_tree_t* tree;
    if (arena) {
        tree = (myrtx_avl_tree_t*)myrtx_arena_alloc(arena, sizeof(myrtx_avl_tree_t));
    } else {
        tree = (myrtx_avl_tree_t*)malloc(sizeof(myrtx_avl_tree_t));
    }
    
    if (!tree) {
        return NULL;
    }
    
    /* Initialize tree */
    tree->root = NULL;
    tree->size = 0;
    tree->arena = arena;
    tree->compare_func = compare_function;
    tree->user_data = user_data;
    
    return tree;
}

void myrtx_avl_tree_free(myrtx_avl_tree_t* tree,
                        myrtx_avl_free_function free_function,
                        void* user_data) {
    if (!tree) {
        return;
    }
    
    /* Recursively free all nodes */
    free_subtree(tree, tree->root, free_function, user_data);
    
    /* Free tree itself if no arena allocator is used */
    if (!tree->arena) {
        free(tree);
    }
}

bool myrtx_avl_tree_insert(myrtx_avl_tree_t* tree,
                          void* key,
                          void* value,
                          void** existing_value) {
    if (!tree || !key) {
        return false;
    }
    
    bool is_new = false;
    tree->root = insert_node(tree, tree->root, key, value, existing_value, &is_new);
    
    /* Increase entry count if a new entry is inserted */
    if (is_new) {
        tree->size++;
    }
    
    return true;
}

bool myrtx_avl_tree_find(const myrtx_avl_tree_t* tree,
                        const void* key,
                        void** value_out) {
    if (!tree || !key) {
        return false;
    }
    
    myrtx_avl_node_t* node = find_node(tree, tree->root, key);
    
    if (node) {
        if (value_out) {
            *value_out = node->value;
        }
        return true;
    }
    
    return false;
}

bool myrtx_avl_tree_remove(myrtx_avl_tree_t* tree,
                          const void* key,
                          void** key_out,
                          void** value_out) {
    if (!tree || !key) {
        return false;
    }
    
    bool found = false;
    tree->root = remove_node(tree, tree->root, key, key_out, value_out, &found);
    
    /* Decrease entry count if an entry is removed */
    if (found) {
        tree->size--;
    }
    
    return found;
}

size_t myrtx_avl_tree_size(const myrtx_avl_tree_t* tree) {
    return tree ? tree->size : 0;
}

bool myrtx_avl_tree_is_empty(const myrtx_avl_tree_t* tree) {
    return !tree || tree->size == 0;
}

void myrtx_avl_tree_clear(myrtx_avl_tree_t* tree,
                         myrtx_avl_free_function free_function,
                         void* user_data) {
    if (!tree) {
        return;
    }
    
    /* Recursively free all nodes */
    free_subtree(tree, tree->root, free_function, user_data);
    
    /* Reset tree */
    tree->root = NULL;
    tree->size = 0;
}

void myrtx_avl_tree_traverse_inorder(const myrtx_avl_tree_t* tree,
                                    myrtx_avl_visit_function visit_function,
                                    void* user_data) {
    if (!tree || !visit_function || !tree->root) {
        return;
    }
    
    bool continue_traversal = true;
    traverse_inorder_recursive(tree->root, visit_function, user_data, &continue_traversal);
}

void myrtx_avl_tree_traverse_preorder(const myrtx_avl_tree_t* tree,
                                     myrtx_avl_visit_function visit_function,
                                     void* user_data) {
    if (!tree || !visit_function || !tree->root) {
        return;
    }
    
    bool continue_traversal = true;
    traverse_preorder_recursive(tree->root, visit_function, user_data, &continue_traversal);
}

void myrtx_avl_tree_traverse_postorder(const myrtx_avl_tree_t* tree,
                                      myrtx_avl_visit_function visit_function,
                                      void* user_data) {
    if (!tree || !visit_function || !tree->root) {
        return;
    }
    
    bool continue_traversal = true;
    traverse_postorder_recursive(tree->root, visit_function, user_data, &continue_traversal);
}

bool myrtx_avl_tree_min(const myrtx_avl_tree_t* tree,
                       void** key_out,
                       void** value_out) {
    if (!tree || !tree->root) {
        return false;
    }
    
    /* Find node with minimum key */
    myrtx_avl_node_t* min_node = find_min_node(tree->root);
    
    if (min_node) {
        if (key_out) {
            *key_out = min_node->key;
        }
        if (value_out) {
            *value_out = min_node->value;
        }
        return true;
    }
    
    return false;
}

bool myrtx_avl_tree_max(const myrtx_avl_tree_t* tree,
                       void** key_out,
                       void** value_out) {
    if (!tree || !tree->root) {
        return false;
    }
    
    /* Find node with maximum key by going right */
    myrtx_avl_node_t* node = tree->root;
    while (node->right) {
        node = node->right;
    }
    
    if (key_out) {
        *key_out = node->key;
    }
    if (value_out) {
        *value_out = node->value;
    }
    
    return true;
}

size_t myrtx_avl_tree_height(const myrtx_avl_tree_t* tree) {
    if (!tree || !tree->root) {
        return 0;
    }
    
    return node_height(tree->root);
}

bool myrtx_avl_tree_contains(const myrtx_avl_tree_t* tree, const void* key) {
    return myrtx_avl_tree_find(tree, key, NULL);
}

int myrtx_avl_compare_strings(const void* key1, const void* key2, void* user_data) {
    const char* str1 = (const char*)key1;
    const char* str2 = (const char*)key2;
    (void)user_data; /* Unused */
    
    return strcmp(str1, str2);
}

int myrtx_avl_compare_integers(const void* key1, const void* key2, void* user_data) {
    /* Einfache Integer-Vergleichsfunktion */
    (void)user_data; /* Unused parameter */
    
    int int1 = *(const int*)key1;
    int int2 = *(const int*)key2;
    
    return int1 - int2;
} 