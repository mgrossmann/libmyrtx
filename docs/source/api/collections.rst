Collections API
=============

The collections module provides data structures for storing and accessing data efficiently.

AVL Tree
-------

The AVL Tree is a self-balancing binary search tree with O(log n) complexity for insertion, deletion, and search operations.

Types
~~~~~

.. c:type:: myrtx_avl_tree_t

   Opaque structure representing an AVL tree.

.. c:type:: myrtx_avl_node_t

   Opaque structure representing a node in the AVL tree.

.. c:type:: myrtx_avl_compare_function

   Function pointer type for comparing keys in the AVL tree.

   .. code-block:: c

      typedef int (*myrtx_avl_compare_function)(const void* key1, const void* key2, void* user_data);

.. c:type:: myrtx_avl_free_function

   Function pointer type for freeing keys and values when nodes are removed.

   .. code-block:: c

      typedef void (*myrtx_avl_free_function)(void* key, void* value, void* user_data);

.. c:type:: myrtx_avl_visit_function

   Function pointer type for traversing the tree.

   .. code-block:: c

      typedef bool (*myrtx_avl_visit_function)(const void* key, void* value, void* user_data);

Creation and Destruction
~~~~~~~~~~~~~~~~~~~~~

.. c:function:: myrtx_avl_tree_t* myrtx_avl_tree_create(myrtx_arena_t* arena, myrtx_avl_compare_function compare_function, void* user_data)

   Creates a new AVL tree.

   :param arena: Optional arena allocator (NULL for malloc/free)
   :param compare_function: Function for comparing keys
   :param user_data: User-defined data passed to the compare function
   :return: Pointer to the new AVL tree or NULL on error

.. c:function:: void myrtx_avl_tree_free(myrtx_avl_tree_t* tree, myrtx_avl_free_function free_function, void* user_data)

   Frees an AVL tree.

   :param tree: Pointer to the AVL tree
   :param free_function: Optional function to free keys and values (NULL if not needed)
   :param user_data: User-defined data for the free function

Insertion and Removal
~~~~~~~~~~~~~~~~~~~

.. c:function:: bool myrtx_avl_tree_insert(myrtx_avl_tree_t* tree, void* key, void* value, void** existing_value)

   Inserts or updates a key-value pair in the AVL tree.

   :param tree: Pointer to the AVL tree
   :param key: Pointer to the key
   :param value: Pointer to the value
   :param existing_value: Pointer to store the previous value (NULL if not needed)
   :return: true on success, false on error

.. c:function:: bool myrtx_avl_tree_remove(myrtx_avl_tree_t* tree, const void* key, void** key_out, void** value_out)

   Removes an entry from the AVL tree.

   :param tree: Pointer to the AVL tree
   :param key: Pointer to the key to remove
   :param key_out: Pointer to store the removed key (NULL if not needed)
   :param value_out: Pointer to store the removed value (NULL if not needed)
   :return: true if the key was found and removed, false otherwise

Lookup Functions
~~~~~~~~~~~~~

.. c:function:: bool myrtx_avl_tree_find(const myrtx_avl_tree_t* tree, const void* key, void** value_out)

   Searches for a value in the AVL tree by key.

   :param tree: Pointer to the AVL tree
   :param key: Pointer to the key to search for
   :param value_out: Pointer to store the found value (NULL if not needed)
   :return: true if the key was found, false otherwise

.. c:function:: bool myrtx_avl_tree_contains(const myrtx_avl_tree_t* tree, const void* key)

   Checks if the AVL tree contains a specific key.

   :param tree: Pointer to the AVL tree
   :param key: Pointer to the key to search for
   :return: true if the key was found, false otherwise

Traversal Functions
~~~~~~~~~~~~~~~

.. c:function:: void myrtx_avl_tree_traverse_inorder(const myrtx_avl_tree_t* tree, myrtx_avl_visit_function visit_function, void* user_data)

   Traverses the AVL tree in-order.

   :param tree: Pointer to the AVL tree
   :param visit_function: Function called for each node
   :param user_data: User-defined data for the visit function

.. c:function:: void myrtx_avl_tree_traverse_preorder(const myrtx_avl_tree_t* tree, myrtx_avl_visit_function visit_function, void* user_data)

   Traverses the AVL tree in pre-order.

   :param tree: Pointer to the AVL tree
   :param visit_function: Function called for each node
   :param user_data: User-defined data for the visit function

.. c:function:: void myrtx_avl_tree_traverse_postorder(const myrtx_avl_tree_t* tree, myrtx_avl_visit_function visit_function, void* user_data)

   Traverses the AVL tree in post-order.

   :param tree: Pointer to the AVL tree
   :param visit_function: Function called for each node
   :param user_data: User-defined data for the visit function

Utility Functions
~~~~~~~~~~~~~

.. c:function:: size_t myrtx_avl_tree_size(const myrtx_avl_tree_t* tree)

   Returns the number of entries in the AVL tree.

   :param tree: Pointer to the AVL tree
   :return: Number of entries

.. c:function:: bool myrtx_avl_tree_is_empty(const myrtx_avl_tree_t* tree)

   Checks if the AVL tree is empty.

   :param tree: Pointer to the AVL tree
   :return: true if the tree is empty, false otherwise

.. c:function:: size_t myrtx_avl_tree_height(const myrtx_avl_tree_t* tree)

   Returns the height of the AVL tree.

   :param tree: Pointer to the AVL tree
   :return: Height of the tree (0 for empty tree)

Min/Max Functions
~~~~~~~~~~~~~

.. c:function:: bool myrtx_avl_tree_min(const myrtx_avl_tree_t* tree, void** key_out, void** value_out)

   Finds the smallest key in the AVL tree.

   :param tree: Pointer to the AVL tree
   :param key_out: Pointer to store the smallest key (NULL if not needed)
   :param value_out: Pointer to store the associated value (NULL if not needed)
   :return: true if the tree is not empty, false otherwise

.. c:function:: bool myrtx_avl_tree_max(const myrtx_avl_tree_t* tree, void** key_out, void** value_out)

   Finds the largest key in the AVL tree.

   :param tree: Pointer to the AVL tree
   :param key_out: Pointer to store the largest key (NULL if not needed)
   :param value_out: Pointer to store the associated value (NULL if not needed)
   :return: true if the tree is not empty, false otherwise

Predefined Comparison Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: int myrtx_avl_compare_strings(const void* key1, const void* key2, void* user_data)

   Standard comparison function for string keys.

   :param key1: Pointer to the first string
   :param key2: Pointer to the second string
   :param user_data: Not used
   :return: Comparison result as from strcmp

.. c:function:: int myrtx_avl_compare_integers(const void* key1, const void* key2, void* user_data)

   Standard comparison function for integer keys.

   :param key1: Pointer to the first integer
   :param key2: Pointer to the second integer
   :param user_data: Not used
   :return: Comparison result (<0, 0, >0)

Hash Table
---------

The Hash Table is a high-performance implementation for key-value storage with constant time complexity for most operations.

.. note:: 
   
   For complete Hash Table API documentation, see the relevant section. 