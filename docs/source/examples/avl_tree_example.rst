AVL Tree Example
================

This example demonstrates how to use the AVL tree data structure in libmyrtx for ordered key-value storage.

Complete Example
--------------

Below is a complete example showing how to create an AVL tree, insert key-value pairs, search for values, and traverse the tree.

.. code-block:: c

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    
    #include "libmyrtx/avl_tree.h"
    #include "libmyrtx/arena.h"
    
    int main() {
        // Initialize arena allocator
        myrtx_arena_t arena = {0};
        myrtx_arena_init(&arena, 0);
        
        // Create AVL tree with string keys
        myrtx_avl_tree_t* tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
        
        printf("Inserting key-value pairs...\n");
        
        // Insert string-string pairs
        myrtx_avl_tree_insert(tree, "Fifty", "50", NULL);
        myrtx_avl_tree_insert(tree, "Twenty", "20", NULL);
        myrtx_avl_tree_insert(tree, "Seventy", "70", NULL);
        myrtx_avl_tree_insert(tree, "Ten", "10", NULL);
        myrtx_avl_tree_insert(tree, "Thirty", "30", NULL);
        myrtx_avl_tree_insert(tree, "Sixty", "60", NULL);
        myrtx_avl_tree_insert(tree, "Eighty", "80", NULL);
        
        printf("Tree size: %zu\n", myrtx_avl_tree_size(tree));
        
        // Look up a value
        void* value;
        if (myrtx_avl_tree_find(tree, "Thirty", &value)) {
            printf("Found value for key 'Thirty': %s\n", (char*)value);
        } else {
            printf("Key 'Thirty' not found\n");
        }
        
        // Check if a key exists
        if (myrtx_avl_tree_contains(tree, "Ninety")) {
            printf("Key 'Ninety' exists\n");
        } else {
            printf("Key 'Ninety' does not exist\n");
        }
        
        // Define a callback function for traversal
        printf("\nTraversing the tree in order:\n");
        myrtx_avl_tree_traverse_inorder(tree, 
            // Inline callback function
            (bool (*)(const void*, void*, void*))
            ((bool (const void* key, void* value, void* user_data) {
                printf("  %s: %s\n", (char*)key, (char*)value);
                return true;  // Continue traversal
            })), 
            NULL);
        
        // Find min and max
        const void* min_key;
        void* min_value;
        if (myrtx_avl_tree_min(tree, &min_key, &min_value)) {
            printf("\nMinimum key: %s, value: %s\n", (char*)min_key, (char*)min_value);
        }
        
        const void* max_key;
        void* max_value;
        if (myrtx_avl_tree_max(tree, &max_key, &max_value)) {
            printf("Maximum key: %s, value: %s\n", (char*)max_key, (char*)max_value);
        }
        
        // Remove a key
        const void* removed_key;
        void* removed_value;
        if (myrtx_avl_tree_remove(tree, "Thirty", &removed_key, &removed_value)) {
            printf("\nRemoved key: %s, value: %s\n", (char*)removed_key, (char*)removed_value);
            printf("Tree size after removal: %zu\n", myrtx_avl_tree_size(tree));
        }
        
        // Clean up
        myrtx_avl_tree_free(tree, NULL, NULL);  // No custom free needed with arena
        myrtx_arena_cleanup(&arena);
        
        return 0;
    }

Expected Output
--------------

When running this example, you should see output similar to:

.. code-block:: text

    Inserting key-value pairs...
    Tree size: 7
    Found value for key 'Thirty': 30
    Key 'Ninety' does not exist
    
    Traversing the tree in order:
      Eighty: 80
      Fifty: 50
      Seventy: 70
      Sixty: 60
      Ten: 10
      Thirty: 30
      Twenty: 20
    
    Minimum key: Eighty, value: 80
    Maximum key: Twenty, value: 20
    
    Removed key: Thirty, value: 30
    Tree size after removal: 6

Key Concepts Demonstrated
-----------------------

1. **Tree Creation and Memory Management**

   The example uses the arena allocator for memory management:

   .. code-block:: c

      myrtx_arena_t arena = {0};
      myrtx_arena_init(&arena, 0);
      myrtx_avl_tree_t* tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);

2. **Inserting Elements**

   Adding key-value pairs to the tree:

   .. code-block:: c

      myrtx_avl_tree_insert(tree, "Fifty", "50", NULL);
      myrtx_avl_tree_insert(tree, "Twenty", "20", NULL);

3. **Looking up Elements**

   Finding elements by key:

   .. code-block:: c

      void* value;
      if (myrtx_avl_tree_find(tree, "Thirty", &value)) {
          printf("Found value: %s\n", (char*)value);
      }

4. **Traversing in Order**

   Walking through elements in sorted order:

   .. code-block:: c

      myrtx_avl_tree_traverse_inorder(tree, callback_function, NULL);

5. **Finding Min/Max Values**

   Retrieving the smallest and largest keys:

   .. code-block:: c

      const void* min_key;
      void* min_value;
      myrtx_avl_tree_min(tree, &min_key, &min_value);

6. **Removing Elements**

   Deleting elements from the tree:

   .. code-block:: c

      myrtx_avl_tree_remove(tree, "Thirty", &removed_key, &removed_value);

7. **Cleanup**

   Properly freeing resources:

   .. code-block:: c

      myrtx_avl_tree_free(tree, NULL, NULL);  
      myrtx_arena_cleanup(&arena);

Building and Running
-----------------

To build and run this example:

1. Ensure libmyrtx is properly installed or available in your project
2. Create a C file with the code above (e.g., `avl_example.c`)
3. Compile with your C compiler, linking against libmyrtx:

   .. code-block:: bash

      gcc -o avl_example avl_example.c -lmyrtx

4. Run the executable:

   .. code-block:: bash

      ./avl_example

Further Exploration
----------------

Try modifying the example to:

- Use integer keys instead of strings
- Implement a custom comparison function
- Store complex structures as values
- Perform a range query by traversing between two keys
- Implement a simple dictionary application 