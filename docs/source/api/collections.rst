Collections API
==============

The collections module provides data structures for storing and accessing data efficiently.

AVL Tree
--------

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
~~~~~~~~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~

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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

Types
~~~~~

.. c:type:: myrtx_hashtable_t

   Opaque structure representing a hash table.

.. c:type:: myrtx_hashtable_entry_t

   Opaque structure representing an entry in the hash table.

.. c:type:: myrtx_hashtable_hash_function

   Function pointer type for computing hash values.

   .. code-block:: c

      typedef uint64_t (*myrtx_hashtable_hash_function)(const void* key, size_t key_size, void* user_data);

.. c:type:: myrtx_hashtable_compare_function

   Function pointer type for comparing keys in the hash table.

   .. code-block:: c

      typedef bool (*myrtx_hashtable_compare_function)(const void* key1, const void* key2, size_t key_size, void* user_data);

.. c:type:: myrtx_hashtable_free_function

   Function pointer type for freeing keys and values when entries are removed.

   .. code-block:: c

      typedef void (*myrtx_hashtable_free_function)(void* key, void* value, void* user_data);

Creation and Destruction
~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: myrtx_hashtable_t* myrtx_hashtable_create(myrtx_allocator_t *allocator, myrtx_hashtable_hash_function hash_function, myrtx_hashtable_compare_function compare_function, void* user_data)

   Creates a new hash table.

   :param allocator: The allocator to use for memory management
   :param hash_function: Function for computing hash values
   :param compare_function: Function for comparing keys
   :param user_data: User-defined data passed to the hash and compare functions
   :return: Pointer to the new hash table or NULL on error

.. c:function:: void myrtx_hashtable_destroy(myrtx_hashtable_t *table, myrtx_hashtable_free_function free_function, void* user_data)

   Frees a hash table.

   :param table: Pointer to the hash table
   :param free_function: Optional function to free keys and values (NULL if not needed)
   :param user_data: User-defined data for the free function

Insertion and Removal
~~~~~~~~~~~~~~~~~~~~

.. c:function:: bool myrtx_hashtable_insert(myrtx_hashtable_t *table, const void *key, size_t key_size, void *value, void** existing_value)

   Inserts or updates a key-value pair in the hash table.

   :param table: Pointer to the hash table
   :param key: Pointer to the key
   :param key_size: Size of the key in bytes
   :param value: Pointer to the value
   :param existing_value: Pointer to store the previous value (NULL if not needed)
   :return: true on success, false on error

.. c:function:: bool myrtx_hashtable_remove(myrtx_hashtable_t *table, const void *key, size_t key_size, void** key_out, void** value_out)

   Removes an entry from the hash table.

   :param table: Pointer to the hash table
   :param key: Pointer to the key to remove
   :param key_size: Size of the key in bytes
   :param key_out: Pointer to store the removed key (NULL if not needed)
   :param value_out: Pointer to store the removed value (NULL if not needed)
   :return: true if the key was found and removed, false otherwise

Lookup Functions
~~~~~~~~~~~~~~~

.. c:function:: bool myrtx_hashtable_get(myrtx_hashtable_t *table, const void *key, size_t key_size, void** value_out)

   Searches for a value in the hash table by key.

   :param table: Pointer to the hash table
   :param key: Pointer to the key to search for
   :param key_size: Size of the key in bytes
   :param value_out: Pointer to store the found value (NULL if not needed)
   :return: true if the key was found, false otherwise

.. c:function:: bool myrtx_hashtable_contains(myrtx_hashtable_t *table, const void *key, size_t key_size)

   Checks if the hash table contains a specific key.

   :param table: Pointer to the hash table
   :param key: Pointer to the key to search for
   :param key_size: Size of the key in bytes
   :return: true if the key was found, false otherwise

Utility Functions
~~~~~~~~~~~~~~~

.. c:function:: size_t myrtx_hashtable_size(const myrtx_hashtable_t *table)

   Returns the number of entries in the hash table.

   :param table: Pointer to the hash table
   :return: Number of entries

.. c:function:: bool myrtx_hashtable_is_empty(const myrtx_hashtable_t *table)

   Checks if the hash table is empty.

   :param table: Pointer to the hash table
   :return: true if the table is empty, false otherwise

.. c:function:: size_t myrtx_hashtable_capacity(const myrtx_hashtable_t *table)

   Returns the current capacity of the hash table.

   :param table: Pointer to the hash table
   :return: Current capacity

Predefined Hash Functions
~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: uint64_t myrtx_hashtable_hash_string(const void* key, size_t key_size, void* user_data)

   Standard hash function for string keys.

   :param key: Pointer to the string
   :param key_size: Size of the string including null terminator
   :param user_data: Not used
   :return: Hash value for the string

.. c:function:: uint64_t myrtx_hashtable_hash_integer(const void* key, size_t key_size, void* user_data)

   Standard hash function for integer keys.

   :param key: Pointer to the integer
   :param key_size: Size of the integer (must be sizeof(int))
   :param user_data: Not used
   :return: Hash value for the integer

Example
~~~~~~~

.. code-block:: c

   #include <myrtx/hashtable.h>
   #include <myrtx/allocator.h>

   int main() {
       myrtx_allocator_t *allocator = myrtx_allocator_create_default();
       myrtx_hashtable_t *table = myrtx_hashtable_create(
           allocator,
           myrtx_hashtable_hash_string,
           myrtx_hashtable_compare_strings,
           NULL
       );

       const char *key = "example";
       int value = 42;

       myrtx_hashtable_insert(table, key, strlen(key) + 1, &value, NULL);
       int *retrieved;
       if (myrtx_hashtable_get(table, key, strlen(key) + 1, &retrieved)) {
           printf("Value: %d\n", *retrieved);
       }

       myrtx_hashtable_destroy(table, NULL, NULL);
       myrtx_allocator_destroy(allocator);
       return 0;
   } 