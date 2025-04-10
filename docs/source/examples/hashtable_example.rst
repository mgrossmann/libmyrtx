Hash Table Example
================

This example demonstrates the basic usage of the hash table implementation in libmyrtx.

Overview
--------

The example shows how to:

- Create and destroy a hash table
- Insert key-value pairs
- Retrieve values by key
- Handle memory management

Code Example
-----------

.. code-block:: c

   #include <myrtx/hashtable.h>
   #include <myrtx/allocator.h>
   #include <stdio.h>
   #include <string.h>

   int main() {
       // Create a default allocator
       myrtx_allocator_t *allocator = myrtx_allocator_create_default();
       if (!allocator) {
           fprintf(stderr, "Failed to create allocator\n");
           return 1;
       }

       // Create a hash table with string keys
       myrtx_hashtable_t *table = myrtx_hashtable_create(
           allocator,
           myrtx_hashtable_hash_string,
           myrtx_hashtable_compare_strings,
           NULL
       );
       if (!table) {
           fprintf(stderr, "Failed to create hash table\n");
           myrtx_allocator_destroy(allocator);
           return 1;
       }

       // Insert some key-value pairs
       const char *keys[] = {"one", "two", "three"};
       int values[] = {1, 2, 3};

       for (size_t i = 0; i < 3; i++) {
           if (!myrtx_hashtable_insert(table, keys[i], strlen(keys[i]) + 1, &values[i], NULL)) {
               fprintf(stderr, "Failed to insert key: %s\n", keys[i]);
           }
       }

       // Retrieve and print values
       for (size_t i = 0; i < 3; i++) {
           int *value;
           if (myrtx_hashtable_get(table, keys[i], strlen(keys[i]) + 1, &value)) {
               printf("%s: %d\n", keys[i], *value);
           } else {
               printf("Key not found: %s\n", keys[i]);
           }
       }

       // Clean up
       myrtx_hashtable_destroy(table, NULL, NULL);
       myrtx_allocator_destroy(allocator);
       return 0;
   }

Explanation
-----------

1. **Allocator Creation**
   - We start by creating a default allocator that will be used for memory management.

2. **Hash Table Creation**
   - Create a hash table with string keys using predefined hash and compare functions.
   - The table is configured to use string keys with `myrtx_hashtable_hash_string` and `myrtx_hashtable_compare_strings`.

3. **Insertion**
   - Insert three key-value pairs where keys are strings and values are integers.
   - The `strlen(key) + 1` is used to include the null terminator in the key size.

4. **Retrieval**
   - Retrieve and print the values for each key.
   - The `myrtx_hashtable_get` function returns true if the key was found.

5. **Cleanup**
   - Properly destroy the hash table and allocator to free all resources.

Compilation
----------

To compile this example, use:

.. code-block:: bash

   gcc -o hashtable_example hashtable_example.c -lmyrtx

Expected Output
--------------

.. code-block:: text

   one: 1
   two: 2
   three: 3 