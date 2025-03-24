Memory API
==========

The memory module implements efficient memory management systems, with a focus on the Arena Allocator.

Arena Allocator
--------------

The Arena Allocator is a powerful, region-based memory allocator that simplifies memory management in C applications.

Types
~~~~~

.. c:type:: myrtx_arena_t

   Opaque structure representing an arena allocator.

   .. code-block:: c

      typedef struct myrtx_arena {
          // Internal details
      } myrtx_arena_t;

.. c:type:: myrtx_scratch_arena_t

   Container for temporary arena memory regions with automatic reset.

   .. code-block:: c

      typedef struct myrtx_scratch_arena {
          myrtx_arena_t* arena;     // Arena being used
          myrtx_arena_t* parent;    // Parent arena, if any
          size_t marker;            // Position for reset
      } myrtx_scratch_arena_t;

Initialization and Cleanup
~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: bool myrtx_arena_init(myrtx_arena_t* arena, size_t block_size)

   Initializes an arena allocator.

   :param arena: Pointer to an uninitialized arena structure
   :param block_size: Size of each memory block in bytes (0 for default size)
   :return: true on success, false on error

.. c:function:: void myrtx_arena_reset(myrtx_arena_t* arena)

   Resets an arena by freeing all allocated memory blocks.

   :param arena: Pointer to an initialized arena

.. c:function:: void myrtx_arena_free(myrtx_arena_t* arena)

   Frees all resources used by the arena.

   :param arena: Pointer to an initialized arena

Memory Allocation
~~~~~~~~~~~~~~

.. c:function:: void* myrtx_arena_alloc(myrtx_arena_t* arena, size_t size)

   Allocates memory from the arena.

   :param arena: Pointer to an initialized arena
   :param size: Number of bytes to allocate
   :return: Pointer to the allocated memory or NULL on error

.. c:function:: void* myrtx_arena_calloc(myrtx_arena_t* arena, size_t size)

   Allocates memory from the arena and initializes it to zero.

   :param arena: Pointer to an initialized arena
   :param size: Number of bytes to allocate
   :return: Pointer to the allocated memory or NULL on error

.. c:function:: void* myrtx_arena_realloc(myrtx_arena_t* arena, void* ptr, size_t old_size, size_t new_size)

   Resizes a previously allocated memory block.

   :param arena: Pointer to an initialized arena
   :param ptr: Pointer to previously allocated memory
   :param old_size: Size of the previously allocated memory block
   :param new_size: New size in bytes
   :return: Pointer to the newly allocated memory or NULL on error

.. c:function:: void* myrtx_arena_alloc_aligned(myrtx_arena_t* arena, size_t size, size_t alignment)

   Allocates aligned memory from the arena.

   :param arena: Pointer to an initialized arena
   :param size: Number of bytes to allocate
   :param alignment: Alignment in bytes (must be a power of two)
   :return: Pointer to the allocated aligned memory or NULL on error

Temporary Arenas
~~~~~~~~~~~~~~

.. c:function:: size_t myrtx_arena_temp_begin(myrtx_arena_t* arena)

   Marks the current state of an arena for temporary use.

   :param arena: Pointer to an initialized arena
   :return: Marker for the current position

.. c:function:: void myrtx_arena_temp_end(myrtx_arena_t* arena, size_t marker)

   Resets an arena to a previously marked state.

   :param arena: Pointer to an initialized arena
   :param marker: Marker position returned by myrtx_arena_temp_begin

Scratch Arenas
~~~~~~~~~~~~

.. c:function:: void myrtx_scratch_begin(myrtx_scratch_arena_t* scratch, myrtx_arena_t* parent)

   Begins a scratch arena session with an existing arena.

   :param scratch: Pointer to an uninitialized scratch arena structure
   :param parent: Pointer to a parent arena (or NULL for a new arena)

.. c:function:: void myrtx_scratch_end(myrtx_scratch_arena_t* scratch)

   Ends a scratch arena session and frees temporary memory.

   :param scratch: Pointer to an initialized scratch arena structure

Helper Functions
~~~~~~~~~~~~

.. c:function:: char* myrtx_arena_strdup(myrtx_arena_t* arena, const char* str)

   Duplicates a string into arena memory.

   :param arena: Pointer to an initialized arena
   :param str: Null-terminated string to duplicate
   :return: Pointer to the duplicated string or NULL on error

.. c:function:: char* myrtx_arena_strndup(myrtx_arena_t* arena, const char* str, size_t n)

   Duplicates the first n characters of a string into arena memory.

   :param arena: Pointer to an initialized arena
   :param str: String to duplicate
   :param n: Maximum number of characters to duplicate
   :return: Pointer to the duplicated string or NULL on error

.. c:function:: void* myrtx_arena_memdup(myrtx_arena_t* arena, const void* ptr, size_t size)

   Duplicates a memory block into arena memory.

   :param arena: Pointer to an initialized arena
   :param ptr: Memory block to duplicate
   :param size: Size of the memory block in bytes
   :return: Pointer to the duplicated memory block or NULL on error

Statistics and Information
~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: size_t myrtx_arena_total_allocated(const myrtx_arena_t* arena)

   Returns the total size of memory allocated by an arena.

   :param arena: Pointer to an initialized arena
   :return: Total size in bytes 