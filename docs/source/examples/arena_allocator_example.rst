Arena Allocator Example
=====================

This example demonstrates the use of the Arena Allocator in various scenarios, including regular arena, temporary arena, and scratch arena.

.. code-block:: c

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <myrtx/memory.h>
    
    // Structure for test data
    typedef struct {
        int id;
        char* name;
        float value;
    } item_t;
    
    // Demo for regular arena
    void demo_regular_arena() {
        printf("\n--- Demo: Regular Arena ---\n");
        
        // Create arena
        myrtx_arena_t arena;
        if (!myrtx_arena_init(&arena, 1024)) {  // 1KB block size
            fprintf(stderr, "Error initializing arena\n");
            return;
        }
        
        // Allocate some items
        const int num_items = 5;
        item_t* items = (item_t*)myrtx_arena_calloc(&arena, num_items * sizeof(item_t));
        
        // Initialize items
        for (int i = 0; i < num_items; i++) {
            items[i].id = i + 1;
            items[i].value = (float)(i * 10.5);
            
            // Allocate name (also from the arena)
            char buffer[32];
            sprintf(buffer, "Item %d", i + 1);
            items[i].name = myrtx_arena_strdup(&arena, buffer);
        }
        
        // Print items
        printf("Allocated items:\n");
        for (int i = 0; i < num_items; i++) {
            printf("  Item %d: id=%d, name='%s', value=%.2f\n", 
                   i, items[i].id, items[i].name, items[i].value);
        }
        
        // Arena statistics
        printf("Total memory usage: %zu bytes\n", myrtx_arena_total_allocated(&arena));
        
        // Reset and reuse arena
        printf("\nResetting arena and reusing...\n");
        myrtx_arena_reset(&arena);
        
        // New item after reset
        item_t* new_item = (item_t*)myrtx_arena_alloc(&arena, sizeof(item_t));
        new_item->id = 42;
        new_item->name = myrtx_arena_strdup(&arena, "After reset");
        new_item->value = 99.9f;
        
        printf("New item: id=%d, name='%s', value=%.2f\n", 
               new_item->id, new_item->name, new_item->value);
        printf("New memory usage: %zu bytes\n", myrtx_arena_total_allocated(&arena));
        
        // Free arena completely
        myrtx_arena_free(&arena);
    }
    
    // Demo for temporary arena
    void demo_temp_arena() {
        printf("\n--- Demo: Temporary Arena ---\n");
        
        myrtx_arena_t arena;
        if (!myrtx_arena_init(&arena, 0)) {
            fprintf(stderr, "Error initializing arena\n");
            return;
        }
        
        // Allocate permanent memory
        char* permanent_data = myrtx_arena_strdup(&arena, "Permanent data");
        printf("Permanent memory allocated: '%s'\n", permanent_data);
        
        // Save marker for temporary use
        size_t marker = myrtx_arena_temp_begin(&arena);
        printf("Temporary allocation begins, marker set\n");
        
        // Allocate temporary memory
        char* temp_data1 = myrtx_arena_strdup(&arena, "Temporary data 1");
        char* temp_data2 = myrtx_arena_strdup(&arena, "Temporary data 2");
        printf("Temporary memory allocated: '%s', '%s'\n", temp_data1, temp_data2);
        printf("Total memory usage: %zu bytes\n", myrtx_arena_total_allocated(&arena));
        
        // Free temporary memory
        myrtx_arena_temp_end(&arena, marker);
        printf("Temporary memory freed\n");
        
        // Try to access temporary memory (unsafe, just for demonstration!)
        printf("Warning: Accessing freed temporary memory (not recommended!):\n");
        printf("  temp_data1 might contain garbage: '%s'\n", temp_data1);
        
        // Permanent memory should still be intact
        printf("Permanent memory should be intact: '%s'\n", permanent_data);
        printf("Memory usage after temp_end: %zu bytes\n", myrtx_arena_total_allocated(&arena));
        
        // Free arena completely
        myrtx_arena_free(&arena);
    }
    
    // Demo for scratch arena
    void demo_scratch_arena() {
        printf("\n--- Demo: Scratch Arena ---\n");
        
        // Create permanent arena
        myrtx_arena_t permanent_arena;
        if (!myrtx_arena_init(&permanent_arena, 0)) {
            fprintf(stderr, "Error initializing permanent arena\n");
            return;
        }
        
        // Operation cycle 1: Standalone scratch arena
        {
            printf("\nOperation cycle 1: Standalone scratch arena\n");
            
            myrtx_scratch_arena_t scratch;
            myrtx_scratch_begin(&scratch, NULL);  // Standalone arena (no parent)
            
            // Allocate memory from the scratch arena
            char* temp_buffer = (char*)myrtx_arena_alloc(scratch.arena, 100);
            strcpy(temp_buffer, "Data in standalone scratch arena");
            printf("Scratch arena data: '%s'\n", temp_buffer);
            
            // Some permanent results need to be copied to the permanent arena
            char* result = myrtx_arena_strdup(&permanent_arena, "Result from cycle 1");
            printf("Permanent result: '%s'\n", result);
            
            // End scratch arena session
            myrtx_scratch_end(&scratch);
            printf("Scratch arena freed\n");
            
            // temp_buffer is now invalid
        }
        
        // Operation cycle 2: Child scratch arena
        {
            printf("\nOperation cycle 2: Child scratch arena\n");
            
            myrtx_scratch_arena_t scratch;
            myrtx_scratch_begin(&scratch, &permanent_arena);  // As child of permanent arena
            
            // Allocate memory from the scratch arena
            char* temp_buffer = (char*)myrtx_arena_alloc(scratch.arena, 100);
            strcpy(temp_buffer, "Data in child scratch arena");
            printf("Scratch arena data: '%s'\n", temp_buffer);
            
            // We can still allocate to the permanent arena
            char* result = myrtx_arena_strdup(&permanent_arena, "Result from cycle 2");
            printf("Permanent result: '%s'\n", result);
            
            // End scratch arena session
            myrtx_scratch_end(&scratch);
            printf("Scratch arena freed\n");
        }
        
        // Now we can access the permanent results
        printf("\nPermanent arena is still intact\n");
        
        // Free arena completely
        myrtx_arena_free(&permanent_arena);
    }
    
    // Demo for nested temporary arenas
    void demo_nested_temp() {
        printf("\n--- Demo: Nested Temporary Arenas ---\n");
        
        myrtx_arena_t arena;
        if (!myrtx_arena_init(&arena, 0)) {
            fprintf(stderr, "Error initializing arena\n");
            return;
        }
        
        // Base marker
        size_t marker_outer = myrtx_arena_temp_begin(&arena);
        char* data_outer = myrtx_arena_strdup(&arena, "Outer data");
        printf("Outer data allocated: '%s'\n", data_outer);
        
        // Nested temporary region
        {
            size_t marker_inner = myrtx_arena_temp_begin(&arena);
            char* data_inner = myrtx_arena_strdup(&arena, "Inner data");
            printf("Inner data allocated: '%s'\n", data_inner);
            
            // Free inner region
            myrtx_arena_temp_end(&arena, marker_inner);
            printf("Inner temporary region freed\n");
            
            // data_inner is now invalid, data_outer should still be valid
            printf("Outer data should still be valid: '%s'\n", data_outer);
        }
        
        // Free outer region
        myrtx_arena_temp_end(&arena, marker_outer);
        printf("Outer temporary region freed\n");
        
        // Free arena completely
        myrtx_arena_free(&arena);
    }
    
    int main() {
        printf("ARENA ALLOCATOR EXAMPLES\n");
        printf("=======================\n");
        
        demo_regular_arena();
        demo_temp_arena();
        demo_scratch_arena();
        demo_nested_temp();
        
        printf("\nAll demos completed.\n");
        return 0;
    }

Expected Output
-------------

Running the above program should produce output similar to the following:

.. code-block:: text

    ARENA ALLOCATOR EXAMPLES
    =======================
    
    --- Demo: Regular Arena ---
    Allocated items:
      Item 0: id=1, name='Item 1', value=0.00
      Item 1: id=2, name='Item 2', value=10.50
      Item 2: id=3, name='Item 3', value=21.00
      Item 3: id=4, name='Item 4', value=31.50
      Item 4: id=5, name='Item 5', value=42.00
    Total memory usage: 1064 bytes
    
    Resetting arena and reusing...
    New item: id=42, name='After reset', value=99.90
    New memory usage: 28 bytes
    
    --- Demo: Temporary Arena ---
    Permanent memory allocated: 'Permanent data'
    Temporary allocation begins, marker set
    Temporary memory allocated: 'Temporary data 1', 'Temporary data 2'
    Total memory usage: 49 bytes
    Temporary memory freed
    Warning: Accessing freed temporary memory (not recommended!):
      temp_data1 might contain garbage: '[invalid data]'
    Permanent memory should be intact: 'Permanent data'
    Memory usage after temp_end: 17 bytes
    
    --- Demo: Scratch Arena ---
    
    Operation cycle 1: Standalone scratch arena
    Scratch arena data: 'Data in standalone scratch arena'
    Permanent result: 'Result from cycle 1'
    Scratch arena freed
    
    Operation cycle 2: Child scratch arena
    Scratch arena data: 'Data in child scratch arena'
    Permanent result: 'Result from cycle 2'
    Scratch arena freed
    
    Permanent arena is still intact
    
    --- Demo: Nested Temporary Arenas ---
    Outer data allocated: 'Outer data'
    Inner data allocated: 'Inner data'
    Inner temporary region freed
    Outer data should still be valid: 'Outer data'
    Outer temporary region freed
    
    All demos completed.

Compilation and Execution
-----------------------

To compile this example, you can use the following command:

.. code-block:: bash

    gcc -o arena_example arena_example.c -lmyrtx
    
    # Run the example
    ./arena_example

Notes
----

1. **Memory Management**: Note that when using the Arena Allocator, we don't need `free()` calls for individual allocations.

2. **Reuse**: After a `myrtx_arena_reset()`, you can reuse the arena's memory without fragmentation.

3. **Temporary vs. Scratch Arena**: 
   - The main difference is that temporary arenas use markers within a single arena
   - Scratch arenas act as standalone arenas or child arenas of a parent arena

4. **Caution with Freed Memory**: Don't access memory already freed by `myrtx_arena_temp_end()` or `myrtx_scratch_end()`. The example code shows this only for demonstration purposes. 