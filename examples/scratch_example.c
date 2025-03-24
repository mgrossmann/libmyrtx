/**
 * @file scratch_example.c
 * @brief Example of using the MYRTX_WITH_SCRATCH macro
 */

#include "myrtx/memory/arena_allocator.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Structure used for demonstration
typedef struct {
    int id;
    char name[32];
    float value;
} example_item_t;

// Example function using manual scratch arena management
void process_data_manual(myrtx_arena_t* arena, int item_count) {
    printf("\n=== Manual Scratch Arena Management ===\n");
    
    // Start timing
    clock_t start = clock();
    
    // Create and initialize scratch arena
    myrtx_scratch_arena_t scratch = {0};
    if (!myrtx_scratch_begin(&scratch, arena)) {
        printf("Failed to create scratch arena\n");
        return;
    }
    
    // Allocate temporary storage
    example_item_t* items = myrtx_arena_alloc(scratch.arena, item_count * sizeof(example_item_t));
    if (!items) {
        printf("Memory allocation failed\n");
        myrtx_scratch_end(&scratch);
        return;
    }
    
    // Use the temporary memory
    for (int i = 0; i < item_count; i++) {
        items[i].id = i;
        snprintf(items[i].name, sizeof(items[i].name), "Item %d", i);
        items[i].value = (float)i * 1.5f;
    }
    
    // Process the data
    float sum = 0.0f;
    for (int i = 0; i < item_count; i++) {
        sum += items[i].value;
    }
    
    printf("Processed %d items with total value: %.2f\n", item_count, sum);
    
    // Cleanup scratch arena
    myrtx_scratch_end(&scratch);
    
    // End timing
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    printf("Processing time: %.2f ms\n", time_spent);
}

// Example function using MYRTX_WITH_SCRATCH macro
void process_data_with_macro(myrtx_arena_t* arena, int item_count) {
    printf("\n=== Using MYRTX_WITH_SCRATCH Macro ===\n");
    
    // Start timing
    clock_t start = clock();
    
    // Use the MYRTX_WITH_SCRATCH macro
    MYRTX_WITH_SCRATCH(arena, scratch) {
        // Allocate temporary storage
        example_item_t* items = myrtx_arena_alloc(scratch.arena, item_count * sizeof(example_item_t));
        if (!items) {
            printf("Memory allocation failed\n");
            return;
        }
        
        // Use the temporary memory
        for (int i = 0; i < item_count; i++) {
            items[i].id = i;
            snprintf(items[i].name, sizeof(items[i].name), "Item %d", i);
            items[i].value = (float)i * 1.5f;
        }
        
        // Process the data
        float sum = 0.0f;
        for (int i = 0; i < item_count; i++) {
            sum += items[i].value;
        }
        
        printf("Processed %d items with total value: %.2f\n", item_count, sum);
        
        // No need for explicit cleanup - the macro handles it automatically
    } // scratch is automatically freed here
    
    // End timing
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    printf("Processing time: %.2f ms\n", time_spent);
}

// Example of nested scratch arenas with the macro
void nested_scratch_example(myrtx_arena_t* arena) {
    printf("\n=== Nested Scratch Arenas ===\n");
    
    MYRTX_WITH_SCRATCH(arena, outer_scratch) {
        printf("Outer scratch arena created\n");
        
        // Allocate in outer scratch
        char* outer_data = myrtx_arena_alloc(outer_scratch.arena, 128);
        strcpy(outer_data, "Data in outer scratch");
        
        // Create nested scratch arena
        MYRTX_WITH_SCRATCH(arena, inner_scratch) {
            printf("Inner scratch arena created\n");
            
            // Allocate in inner scratch
            char* inner_data = myrtx_arena_alloc(inner_scratch.arena, 128);
            strcpy(inner_data, "Data in inner scratch");
            
            printf("Inner scratch data: %s\n", inner_data);
            
            // Inner scratch is freed here
        }
        
        printf("Outer scratch data after inner scratch is freed: %s\n", outer_data);
        
        // Outer scratch is freed here
    }
    
    printf("All scratch arenas are freed\n");
}

// Example showing early return with scratch arenas
void early_return_example(myrtx_arena_t* arena, int condition) {
    printf("\n=== Early Return Example ===\n");
    
    MYRTX_WITH_SCRATCH(arena, scratch) {
        printf("Scratch arena created\n");
        
        // Allocate some memory
        void* data = myrtx_arena_alloc(scratch.arena, 256);
        printf("Memory allocated in scratch arena\n");
        
        // Return early if the condition is met
        if (condition) {
            printf("Early return triggered (condition = %d)\n", condition);
            return; // The scratch arena is still properly cleaned up
        }
        
        printf("Continuing execution (condition = %d)\n", condition);
        
        // Scratch is freed here
    }
    
    printf("After scratch arena scope\n");
}

int main(void) {
    printf("=== MYRTX_WITH_SCRATCH Example ===\n");
    
    // Initialize main arena
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Failed to initialize arena\n");
        return 1;
    }
    
    // Run examples
    const int item_count = 1000000;
    
    process_data_manual(&arena, item_count);
    process_data_with_macro(&arena, item_count);
    
    nested_scratch_example(&arena);
    
    early_return_example(&arena, 0); // Without early return
    early_return_example(&arena, 1); // With early return
    
    // Free the arena
    myrtx_arena_free(&arena);
    
    printf("\nExample completed successfully\n");
    return 0;
} 