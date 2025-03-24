Arena Allocator Guide
==================

Overview
--------

The Arena Allocator is a powerful, region-based memory management mechanism that provides an efficient alternative to conventional `malloc()` and `free()` functions. It is particularly useful for programs that perform many small allocations or need temporary memory that can be released in bulk.

Why Use an Arena Allocator?
--------------------------

Using an Arena Allocator offers several advantages over conventional allocation methods:

1. **Performance**: Arena-based allocation is significantly faster than repeated `malloc()` calls, especially for many small allocations.
2. **Reduced Fragmentation**: The Arena Allocator significantly reduces memory fragmentation.
3. **Simplified Memory Management**: No need to track and free each individual memory block.
4. **Reduced Overhead**: Less metadata per allocation compared to `malloc()`.
5. **Fast Cleanup**: Entire memory can be reset or freed with a single call.

Basic Usage
----------

The Arena Allocator in the libmyrtx library consists of several components, with the base component being the generic Arena Allocator.

Initializing an Arena
~~~~~~~~~~~~~~~~~~~

To initialize an arena:

.. code-block:: c

    #include <myrtx/memory.h>

    int main() {
        myrtx_arena_t arena;
        
        // Initialize arena with default block size
        if (!myrtx_arena_init(&arena, 0)) {
            fprintf(stderr, "Error initializing arena\n");
            return 1;
        }
        
        // ...code that uses the arena...
        
        // Free the arena when no longer needed
        myrtx_arena_free(&arena);
        
        return 0;
    }

Allocating Memory from the Arena
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once an arena is initialized, you can allocate memory from it:

.. code-block:: c

    // Allocate memory for an int array
    int* numbers = (int*)myrtx_arena_alloc(&arena, 10 * sizeof(int));
    
    // Allocate memory for a string and initialize it to zeros
    char* buffer = (char*)myrtx_arena_calloc(&arena, 100);
    
    // Allocate aligned memory (e.g., for SIMD operations)
    float* aligned_data = (float*)myrtx_arena_alloc_aligned(&arena, 16 * sizeof(float), 16);

Duplicating strings and memory blocks:

.. code-block:: c

    // Duplicate a string
    const char* original = "Hello World";
    char* copy = myrtx_arena_strdup(&arena, original);
    
    // Duplicate a memory block
    int values[] = {1, 2, 3, 4, 5};
    int* values_copy = (int*)myrtx_arena_memdup(&arena, values, sizeof(values));

Important: You don't need to manually free memory allocated from an arena. This happens automatically when you call either `myrtx_arena_reset()` or `myrtx_arena_free()`.

Comparison: Arena Allocator vs. malloc/free
-----------------------------------------

To illustrate the benefits of the Arena Allocator, let's compare code that performs many small allocations:

**With malloc/free:**

.. code-block:: c

    // Traditional approach with malloc/free
    void process_data_traditional(size_t count) {
        char** strings = (char**)malloc(count * sizeof(char*));
        
        for (size_t i = 0; i < count; i++) {
            strings[i] = (char*)malloc(32);  // 32-byte strings
            sprintf(strings[i], "String %zu", i);
            // Processing...
        }
        
        // Free individually
        for (size_t i = 0; i < count; i++) {
            free(strings[i]);
        }
        free(strings);
    }

**With Arena Allocator:**

.. code-block:: c

    // Arena-based approach
    void process_data_arena(size_t count) {
        myrtx_arena_t arena;
        myrtx_arena_init(&arena, 0);
        
        char** strings = (char**)myrtx_arena_alloc(&arena, count * sizeof(char*));
        
        for (size_t i = 0; i < count; i++) {
            strings[i] = (char*)myrtx_arena_alloc(&arena, 32);
            sprintf(strings[i], "String %zu", i);
            // Processing...
        }
        
        // Simple cleanup with one call
        myrtx_arena_free(&arena);
    }

The arena approach offers the following advantages:
- Faster allocation on repeated calls
- Eliminates potential memory leaks from forgotten `free()` calls
- Reduces code complexity for memory management

Temporary Arenas: When and How to Use
-----------------------------------

Temporary arenas are useful when you want to allocate memory for a specific operation and then free everything at once, while keeping the base arena intact.

.. code-block:: c

    void process_chunk(myrtx_arena_t* persistent_arena, const data_t* data) {
        // Store marker for temporary use
        size_t marker = myrtx_arena_temp_begin(persistent_arena);
        
        // Allocate temporary data
        intermediate_result_t* temp = (intermediate_result_t*)myrtx_arena_alloc(
            persistent_arena, data->size * sizeof(intermediate_result_t));
        
        // Perform processing...
        result_t* final_result = compute_result(persistent_arena, temp, data);
        
        // Insert permanent result into the persistent collection
        add_to_results(persistent_arena, final_result);
        
        // Free temporary memory, permanent memory remains
        myrtx_arena_temp_end(persistent_arena, marker);
    }

Typical use cases for temporary arenas:
- Intermediate buffers for parsing algorithms
- Temporary calculations with large data sets
- Multi-stage processing pipelines where intermediate results can be discarded

Scratch Arenas: Short-lived, Automatic Memory Management
------------------------------------------------------

Scratch Arenas are an extension of the temporary arena concept and are particularly useful for short-lived operations where clear visibility of the allocation/deallocation cycle is important.

.. code-block:: c

    result_t* process_request(myrtx_arena_t* main_arena, const request_t* request) {
        // Create scratch arena for this request
        myrtx_scratch_arena_t scratch;
        myrtx_scratch_begin(&scratch, main_arena);  // Use main_arena as parent
        
        // Allocate memory from the scratch arena
        temp_data_t* temp = (temp_data_t*)myrtx_arena_alloc(scratch.arena, 
                                                          sizeof(temp_data_t) * request->count);
        
        // Processing...
        result_t* result = generate_result(scratch.arena, temp, request);
        
        // Copy result to main arena to persist beyond scratch arena lifetime
        result_t* persistent_result = (result_t*)myrtx_arena_memdup(main_arena, result, sizeof(result_t));
        
        // End scratch arena session and free temporary memory
        myrtx_scratch_end(&scratch);
        
        return persistent_result;
    }

When to use Scratch Arenas:
- During a single function operation
- For processing pipelines with clearly defined start and end
- In server applications for handling individual client requests

A More Comprehensive Example: Parser with Arena Allocator
-----------------------------------------------------

Here's a more comprehensive example implementing parsing operations using different arena types:

.. code-block:: c

    typedef struct {
        char* key;
        char* value;
    } key_value_t;

    typedef struct {
        key_value_t* items;
        size_t count;
        size_t capacity;
    } config_t;

    config_t* parse_config_file(myrtx_arena_t* persistent_arena, const char* filename) {
        // Scratch arena for temporary parsing operations
        myrtx_scratch_arena_t scratch;
        myrtx_scratch_begin(&scratch, NULL);  // Standalone arena, not connected to persistent_arena
        
        // Read file into buffer
        FILE* file = fopen(filename, "r");
        if (!file) {
            myrtx_scratch_end(&scratch);
            return NULL;
        }
        
        // Determine file size
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // Allocate file buffer
        char* buffer = (char*)myrtx_arena_alloc(scratch.arena, file_size + 1);
        fread(buffer, 1, file_size, file);
        buffer[file_size] = '\0';
        fclose(file);
        
        // Create configuration object in persistent arena
        config_t* config = (config_t*)myrtx_arena_calloc(persistent_arena, sizeof(config_t));
        config->capacity = 16;  // Initial size
        config->items = (key_value_t*)myrtx_arena_calloc(
            persistent_arena, sizeof(key_value_t) * config->capacity);
        
        // Parse line by line
        char* line = strtok(buffer, "\n");
        while (line) {
            // Set temporary marker for this line
            size_t line_marker = myrtx_arena_temp_begin(scratch.arena);
            
            // Parse line
            char* equals = strchr(line, '=');
            if (equals) {
                // Found key-value pair
                *equals = '\0';  // Replace with null terminator
                
                char* key = line;
                char* value = equals + 1;
                
                // Trim whitespace (simplified version)
                while (*key && isspace(*key)) key++;
                while (*value && isspace(*value)) value++;
                
                char* key_end = key + strlen(key) - 1;
                char* value_end = value + strlen(value) - 1;
                
                while (key_end > key && isspace(*key_end)) *key_end-- = '\0';
                while (value_end > value && isspace(*value_end)) *value_end-- = '\0';
                
                // Increase capacity if needed (in real application, better with separate function)
                if (config->count >= config->capacity) {
                    size_t new_capacity = config->capacity * 2;
                    key_value_t* new_items = (key_value_t*)myrtx_arena_alloc(
                        persistent_arena, sizeof(key_value_t) * new_capacity);
                    
                    memcpy(new_items, config->items, sizeof(key_value_t) * config->count);
                    
                    config->items = new_items;
                    config->capacity = new_capacity;
                }
                
                // Copy to persistent arena
                config->items[config->count].key = myrtx_arena_strdup(persistent_arena, key);
                config->items[config->count].value = myrtx_arena_strdup(persistent_arena, value);
                config->count++;
            }
            
            // Reset temporary parsing buffer
            myrtx_arena_temp_end(scratch.arena, line_marker);
            
            // Get next line
            line = strtok(NULL, "\n");
        }
        
        // Free the entire scratch arena
        myrtx_scratch_end(&scratch);
        
        return config;
    }

Recommendations for Different Arena Types
---------------------------------------

### Regular Arena

Use a standard arena when:
- You're managing a long-lived collection of data
- Memory is needed throughout the program's lifetime
- You only want to explicitly free at the end
- You need your own fine-grained control over reset operations

### Temporary Arena (with temp_begin/temp_end)

Use temporary arena mode when:
- You need temporary memory within an existing arena
- You want to use a bounded region of an arena for a specific operation and then free it
- You have both permanent and temporary memory in the same arena

### Scratch Arena

Use Scratch Arenas when:
- You have a clearly delineated, short-lived operation
- Memory allocations can be associated with a specific function or operation
- You prefer code clarity and explicit begin/end cycles
- You want to choose between fully isolated arenas (with parent=NULL) and child arenas (with a parent)

Performance Optimization with the Arena Allocator
----------------------------------------------

To get the best performance from the Arena Allocator:

1. **Choose Block Size Appropriately**: If you know the typical size and number of allocations, you can adjust the block size accordingly. Too small blocks lead to more overhead, too large blocks may waste memory.

2. **Consider Allocation Patterns**: Arenas work most efficiently with many similar allocations or when data is created and freed in a predictable order.

3. **Use Arena Hierarchy**: Using the parent parameter in Scratch Arenas, you can create hierarchical memory systems that match well with nested function calls.

4. **Be Careful with Large Single Allocations**: Arenas are optimal for many small allocations. For single very large blocks, conventional `malloc()` might be more efficient.

5. **Arena per Thread**: In multi-threaded environments, each thread should use its own arena to avoid synchronization issues.

Conclusion
---------

The Arena Allocator provides a powerful and flexible alternative to conventional memory management techniques. By using the right arena technique for each use case, you can significantly improve both the performance and code quality of your applications.

The libmyrtx library offers a comprehensive toolset with its Arena Allocator, temporary arenas, and Scratch Arenas for various memory management requirements, from simple scripts to complex applications with high-performance demands. 