AVL Tree Guide
=============

This guide explains how to use the AVL Tree implementation in libmyrtx effectively.

What is an AVL Tree?
-----------------

An AVL (Adelson-Velsky and Landis) tree is a self-balancing binary search tree where the difference in height between left and right subtrees (balance factor) is at most 1 for every node. This balancing ensures O(log n) complexity for search, insertion, and deletion operations.

Key features:
- Self-balancing binary search tree
- O(log n) complexity for search, insert, and delete operations
- In-order traversal yields sorted results
- Supports efficient range queries and ordered data access

When to Use an AVL Tree
--------------------

AVL trees are ideal when:
- You need ordered data with fast access
- Your data requires frequent insertions, deletions, and look-ups
- You need to efficiently find the minimum or maximum values
- You need to iterate over items in sorted order

They're particularly useful for:
- Implementing ordered dictionaries
- Index structures in databases
- Range queries (finding all items within a range)
- Maintaining sorted data with frequent modifications

Basic Usage
---------

Creating an AVL Tree
~~~~~~~~~~~~~~~~~

You can create an AVL tree using the arena allocator for memory management or with standard malloc/free:

.. code-block:: c

   // Using arena allocator (recommended)
   myrtx_arena_t arena = {0};
   myrtx_arena_init(&arena, 0);
   myrtx_avl_tree_t* tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);

   // Using malloc/free
   myrtx_avl_tree_t* tree = myrtx_avl_tree_create(NULL, myrtx_avl_compare_strings, NULL);

The second parameter is a comparison function for the keys. The library provides standard comparison functions for strings and integers:

.. code-block:: c

   // For string keys
   myrtx_avl_tree_t* string_tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);

   // For integer keys
   myrtx_avl_tree_t* int_tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_integers, NULL);

Inserting Elements
~~~~~~~~~~~~~~~

To insert elements into the tree:

.. code-block:: c

   int value = 42;
   myrtx_avl_tree_insert(tree, "key", &value, NULL);

If the key already exists, the value is updated and the previous value can be retrieved:

.. code-block:: c

   int new_value = 100;
   void* old_value;
   myrtx_avl_tree_insert(tree, "key", &new_value, &old_value);
   // Now old_value points to the previous value (42)

Finding Elements
~~~~~~~~~~~~~

To find an element by key:

.. code-block:: c

   void* found_value;
   if (myrtx_avl_tree_find(tree, "key", &found_value)) {
       printf("Value: %d\n", *(int*)found_value);
   } else {
       printf("Key not found\n");
   }

You can also check if a key exists without retrieving the value:

.. code-block:: c

   if (myrtx_avl_tree_contains(tree, "key")) {
       printf("Key exists\n");
   }

Removing Elements
~~~~~~~~~~~~~~

To remove an element from the tree:

.. code-block:: c

   myrtx_avl_tree_remove(tree, "key", NULL, NULL);

You can also retrieve the removed key and value:

.. code-block:: c

   void* removed_key;
   void* removed_value;
   if (myrtx_avl_tree_remove(tree, "key", &removed_key, &removed_value)) {
       printf("Removed key: %s, value: %d\n", (char*)removed_key, *(int*)removed_value);
   }

Traversing the Tree
~~~~~~~~~~~~~~~~

The AVL tree supports three traversal orders:

.. code-block:: c

   // Define a callback function
   bool visit_callback(const void* key, void* value, void* user_data) {
       printf("Key: %s, Value: %d\n", (char*)key, *(int*)value);
       return true;  // Return false to stop traversal
   }

   // In-order traversal (sorted by key)
   myrtx_avl_tree_traverse_inorder(tree, visit_callback, NULL);

   // Pre-order traversal
   myrtx_avl_tree_traverse_preorder(tree, visit_callback, NULL);

   // Post-order traversal
   myrtx_avl_tree_traverse_postorder(tree, visit_callback, NULL);

Finding Min/Max Elements
~~~~~~~~~~~~~~~~~~~~

To find the minimum or maximum key:

.. code-block:: c

   void* min_key;
   void* min_value;
   if (myrtx_avl_tree_min(tree, &min_key, &min_value)) {
       printf("Min key: %s, value: %d\n", (char*)min_key, *(int*)min_value);
   }

   void* max_key;
   void* max_value;
   if (myrtx_avl_tree_max(tree, &max_key, &max_value)) {
       printf("Max key: %s, value: %d\n", (char*)max_key, *(int*)max_value);
   }

Advanced Usage
-----------

Custom Comparison Functions
~~~~~~~~~~~~~~~~~~~~~~~

For custom key types, you need to provide a comparison function:

.. code-block:: c

   typedef struct {
       int x;
       int y;
   } Point;

   int compare_points(const void* key1, const void* key2, void* user_data) {
       const Point* p1 = (const Point*)key1;
       const Point* p2 = (const Point*)key2;
       
       // First compare x, then y
       if (p1->x != p2->x) {
           return p1->x - p2->x;
       }
       return p1->y - p2->y;
   }

   // Create tree with custom comparison
   myrtx_avl_tree_t* point_tree = myrtx_avl_tree_create(&arena, compare_points, NULL);

Custom Memory Management
~~~~~~~~~~~~~~~~~~~~

When using the AVL tree without an arena allocator, you need to free the memory manually:

.. code-block:: c

   // Define a free callback
   void free_callback(void* key, void* value, void* user_data) {
       free(key);
       free(value);
   }

   // Free the tree and all its elements
   myrtx_avl_tree_free(tree, free_callback, NULL);

Performance Considerations
----------------------

AVL trees maintain a stricter balance than some other self-balancing trees (like Red-Black trees), which makes them:

- Slightly slower for insertions and deletions (due to more rotations)
- Slightly faster for lookup operations (due to more balanced structure)
- Very efficient for ordered traversal

For best performance:
- Use arena allocators when appropriate to reduce memory allocation overhead
- Choose the right comparison function for your key type
- Consider using a hash table instead if you don't need ordered operations

Common Use Cases
------------

1. **Dictionary with Ordered Iteration**:

   .. code-block:: c

      // Create a dictionary
      myrtx_avl_tree_t* dict = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
      
      // Insert words
      int value1 = 1;
      int value2 = 2;
      myrtx_avl_tree_insert(dict, "apple", &value1, NULL);
      myrtx_avl_tree_insert(dict, "banana", &value2, NULL);
      
      // Print in alphabetical order
      myrtx_avl_tree_traverse_inorder(dict, visit_callback, NULL);

2. **Numeric Range Index**:

   .. code-block:: c

      // Create a tree with numeric keys
      myrtx_avl_tree_t* index = myrtx_avl_tree_create(&arena, myrtx_avl_compare_integers, NULL);
      
      // Insert data
      for (int i = 0; i < 100; i++) {
          int* key = myrtx_arena_alloc(&arena, sizeof(int));
          *key = i;
          myrtx_avl_tree_insert(index, key, data_for_key(i), NULL);
      }
      
      // Find range by traversing from min to threshold
      void* min_key;
      myrtx_avl_tree_min(index, &min_key, NULL);
      // ... then traverse until key > threshold

Conclusion
---------

The AVL tree implementation in libmyrtx provides a robust, efficient, and easy-to-use solution for ordered data storage. By following the patterns described in this guide, you can leverage its strengths for a wide range of applications. 