Hashtable
=========

The hashtable module provides a hash table implementation with the following features:

- Dynamic resizing
- Custom hash functions
- Key-value pair storage
- Thread-safe operations

Data Structures
--------------

.. c:type:: myrtx_hashtable_t

   A hash table structure that stores key-value pairs.

   .. code-block:: c

      typedef struct {
          size_t size;
          size_t capacity;
          myrtx_hashtable_entry_t *entries;
          myrtx_allocator_t *allocator;
      } myrtx_hashtable_t;

Functions
---------

.. c:function:: myrtx_hashtable_t* myrtx_hashtable_create(myrtx_allocator_t *allocator)

   Create a new hash table.

   :param allocator: The allocator to use for memory management
   :return: A pointer to the newly created hash table, or NULL on failure

.. c:function:: void myrtx_hashtable_destroy(myrtx_hashtable_t *table)

   Destroy a hash table and free its resources.

   :param table: The hash table to destroy

.. c:function:: bool myrtx_hashtable_insert(myrtx_hashtable_t *table, const void *key, size_t key_size, void *value)

   Insert a key-value pair into the hash table.

   :param table: The hash table
   :param key: The key to insert
   :param key_size: The size of the key in bytes
   :param value: The value to associate with the key
   :return: true if successful, false otherwise

.. c:function:: void* myrtx_hashtable_get(myrtx_hashtable_t *table, const void *key, size_t key_size)

   Retrieve a value from the hash table by key.

   :param table: The hash table
   :param key: The key to look up
   :param key_size: The size of the key in bytes
   :return: The associated value, or NULL if not found

Example
-------

.. code-block:: c

   #include <myrtx/hashtable.h>
   #include <myrtx/allocator.h>

   int main() {
       myrtx_allocator_t *allocator = myrtx_allocator_create_default();
       myrtx_hashtable_t *table = myrtx_hashtable_create(allocator);

       const char *key = "example";
       int value = 42;

       myrtx_hashtable_insert(table, key, strlen(key) + 1, &value);
       int *retrieved = myrtx_hashtable_get(table, key, strlen(key) + 1);

       if (retrieved) {
           printf("Value: %d\n", *retrieved);
       }

       myrtx_hashtable_destroy(table);
       myrtx_allocator_destroy(allocator);
       return 0;
   } 