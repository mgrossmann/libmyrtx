Hash Table Guide
================

This guide provides a comprehensive overview of the hash table implementation in libmyrtx and how to use it effectively in your applications.

What is a Hash Table?
---------------------

A hash table is a data structure that provides fast key-value lookups with an average time complexity of O(1). The libmyrtx hash table implementation features:

- Generic key-value storage with void pointers
- Linear probing for collision resolution
- Automatic resizing for performance
- Support for custom hash and equality functions
- High load factor with good performance characteristics

When to Use a Hash Table
------------------------

Hash tables are ideal for:

- Fast lookup operations by key
- Data association (mapping keys to values)
- Caching and memoization
- Uniqueness checking
- Sets and dictionaries

They're particularly useful when performance is critical, and you don't need to maintain any order of the elements.

Basic Usage
-----------

Creating a Hash Table
~~~~~~~~~~~~~~~~~~~~~

You can create a hash table using the arena allocator for memory management or with standard malloc/free:

.. code-block:: c

   // Using arena allocator (recommended)
   myrtx_arena_t arena = {0};
   myrtx_arena_init(&arena, 0);
   myrtx_hash_table_t* table = myrtx_hash_table_create(&arena, 0, NULL, NULL);

   // Using malloc/free
   myrtx_hash_table_t* table = myrtx_hash_table_create(NULL, 0, NULL, NULL);

The second parameter is the initial capacity. Passing 0 will use the default capacity. The third and fourth parameters are functions for hashing and equality checking, respectively. If NULL is provided, the default functions (which work on string keys) are used.

Inserting Elements
~~~~~~~~~~~~~~~~~~

To insert elements into the hash table:

.. code-block:: c

   int value = 42;
   myrtx_hash_table_insert(table, "key", &value);

If the key already exists, the value is updated and the function returns false. If a new entry is created, it returns true.

Finding Elements
~~~~~~~~~~~~~~~~

To find an element by key:

.. code-block:: c

   void* value;
   if (myrtx_hash_table_find(table, "key", &value)) {
       printf("Value: %d\n", *(int*)value);
   } else {
       printf("Key not found\n");
   }

Removing Elements
~~~~~~~~~~~~~~~~~

To remove an element from the hash table:

.. code-block:: c

   if (myrtx_hash_table_remove(table, "key")) {
       printf("Key removed\n");
   } else {
       printf("Key not found\n");
   }

Getting Hash Table Statistics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can check the current size and capacity of the hash table:

.. code-block:: c

   size_t size = myrtx_hash_table_size(table);
   size_t capacity = myrtx_hash_table_capacity(table);
   printf("Size: %zu, Capacity: %zu, Load Factor: %f\n", 
          size, capacity, (float)size / capacity);

Advanced Usage
--------------

Custom Hash Functions
~~~~~~~~~~~~~~~~~~~~~

You can provide custom hash functions for your specific key types:

.. code-block:: c

   // Custom hash function for integer keys
   uint64_t hash_int(const void* key) {
       return (uint64_t)(*(int*)key);
   }

   // Custom equality function for integer keys
   bool int_equals(const void* a, const void* b) {
       return *(int*)a == *(int*)b;
   }

   // Create hash table with custom functions
   myrtx_hash_table_t* int_table = myrtx_hash_table_create(
       &arena, 0, hash_int, int_equals);

   // Use integers as keys
   int key1 = 100;
   int value1 = 42;
   myrtx_hash_table_insert(int_table, &key1, &value1);

Iterating Over a Hash Table
~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can iterate over all entries in a hash table:

.. code-block:: c

   // Iterator callback function
   bool iterator_callback(const void* key, void* value, void* user_data) {
       printf("Key: %s, Value: %d\n", (char*)key, *(int*)value);
       return true;  // Return false to stop iteration
   }

   // Iterate over all entries
   myrtx_hash_table_iterate(table, iterator_callback, NULL);

The third parameter is user_data that will be passed to the callback function.

Using with Complex Data Types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For complex key types, you need custom hash and equality functions:

.. code-block:: c

   typedef struct {
       int id;
       char name[32];
   } Person;

   uint64_t hash_person(const void* key) {
       const Person* person = (const Person*)key;
       // Combine hash of id and name
       uint64_t id_hash = (uint64_t)person->id;
       uint64_t name_hash = 0;
       for (int i = 0; person->name[i]; i++) {
           name_hash = name_hash * 31 + person->name[i];
       }
       return id_hash ^ name_hash;
   }

   bool person_equals(const void* a, const void* b) {
       const Person* p1 = (const Person*)a;
       const Person* p2 = (const Person*)b;
       return p1->id == p2->id && strcmp(p1->name, p2->name) == 0;
   }

   // Create hash table for Person keys
   myrtx_hash_table_t* person_table = myrtx_hash_table_create(
       &arena, 0, hash_person, person_equals);

Performance Considerations
---------------------------

The hash table implementation in libmyrtx is designed for performance. Here are some tips to get the best performance:

1. **Choose the right initial capacity**:
   
   If you know approximately how many elements you'll store, set an appropriate initial capacity to avoid resizing.

2. **Use good hash functions**:
   
   The quality of your hash function significantly impacts performance. A good hash function should:
   - Distribute keys uniformly across the hash space
   - Be fast to compute
   - Generate different hash values for different keys

3. **Consider memory usage**:
   
   Hash tables trade memory for speed. The default load factor is around 0.75, which offers a good balance between memory usage and performance.

4. **Use the arena allocator**:
   
   Using the arena allocator can significantly improve performance by reducing the overhead of memory allocation.

Common Use Cases
----------------

1. **Simple Dictionary**:

   .. code-block:: c

      // Create a dictionary
      myrtx_hash_table_t* dict = myrtx_hash_table_create(&arena, 0, NULL, NULL);
      
      // Store values
      const char* value1 = "apple";
      const char* value2 = "banana";
      myrtx_hash_table_insert(dict, "fruit1", (void*)value1);
      myrtx_hash_table_insert(dict, "fruit2", (void*)value2);
      
      // Look up values
      void* value;
      if (myrtx_hash_table_find(dict, "fruit1", &value)) {
          printf("fruit1: %s\n", (char*)value);
      }

2. **Counting Occurrences**:

   .. code-block:: c

      // Count word occurrences
      myrtx_hash_table_t* counter = myrtx_hash_table_create(&arena, 0, NULL, NULL);
      
      const char* words[] = {"apple", "banana", "apple", "orange", "banana", "apple"};
      for (size_t i = 0; i < sizeof(words) / sizeof(words[0]); i++) {
          void* count_ptr;
          if (myrtx_hash_table_find(counter, words[i], &count_ptr)) {
              int* count = (int*)count_ptr;
              (*count)++;
          } else {
              int* count = myrtx_arena_alloc(&arena, sizeof(int));
              *count = 1;
              myrtx_hash_table_insert(counter, words[i], count);
          }
      }
      
      // Print counts
      myrtx_hash_table_iterate(counter, 
          (bool (*)(const void*, void*, void*))((bool(const void* k, void* v, void* ud) {
              printf("%s: %d\n", (char*)k, *(int*)v);
              return true;
          })), NULL);

3. **Cache Implementation**:

   .. code-block:: c

      // Simple function result cache
      myrtx_hash_table_t* cache = myrtx_hash_table_create(&arena, 0, NULL, NULL);
      
      // Function to compute factorial with caching
      int factorial(int n) {
          // Convert n to string key
          char key[16];
          sprintf(key, "%d", n);
          
          // Check cache
          void* result_ptr;
          if (myrtx_hash_table_find(cache, key, &result_ptr)) {
              return *(int*)result_ptr;
          }
          
          // Compute result
          int result = (n <= 1) ? 1 : n * factorial(n - 1);
          
          // Cache result
          int* cached_result = myrtx_arena_alloc(&arena, sizeof(int));
          *cached_result = result;
          myrtx_hash_table_insert(cache, myrtx_strdup(&arena, key), cached_result);
          
          return result;
      }

Conclusion
----------

The hash table implementation in libmyrtx provides a powerful and flexible solution for key-value storage. By following the patterns described in this guide, you can leverage its strengths for a wide range of applications requiring fast lookup operations. 
