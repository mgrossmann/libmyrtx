/**
 * @file context_example.c
 * @brief Example usage of the myrtx Context System
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "myrtx/myrtx.h"

/* Example custom extension */
typedef struct {
    int count;
    float values[10];
    char* name;
} custom_extension_t;

/* Initialize custom extension */
void custom_extension_init(void* data) {
    custom_extension_t* ext = (custom_extension_t*)data;
    ext->count = 0;
    ext->name = NULL;
    printf("Custom extension initialized\n");
}

/* Finalize custom extension */
void custom_extension_finalize(void* data) {
    custom_extension_t* ext = (custom_extension_t*)data;
    if (ext->name) {
        free(ext->name);
    }
    printf("Custom extension finalized\n");
}

/* Register our custom extension */
static int register_custom_extension(void) {
    myrtx_extension_info_t info = {
        .name = "custom_extension",
        .data_size = sizeof(custom_extension_t),
        .initialize = custom_extension_init,
        .finalize = custom_extension_finalize
    };
    
    return myrtx_register_extension(&info);
}

/* Example function that uses a context for allocations */
void context_allocation_example(myrtx_context_t* ctx) {
    printf("\n=== Context Allocation Example ===\n");
    
    /* Allocate memory from the global arena */
    int* global_data = (int*)myrtx_context_alloc(ctx, 100 * sizeof(int));
    if (!global_data) {
        printf("Failed to allocate global memory\n");
        return;
    }
    
    /* Initialize data */
    for (int i = 0; i < 100; i++) {
        global_data[i] = i * 2;
    }
    
    /* Allocate temporary memory */
    float* temp_data = (float*)myrtx_context_temp_alloc(ctx, 50 * sizeof(float));
    if (!temp_data) {
        printf("Failed to allocate temporary memory\n");
        return;
    }
    
    /* Initialize temporary data */
    for (int i = 0; i < 50; i++) {
        temp_data[i] = (float)i * 1.5f;
    }
    
    /* Display some data */
    printf("Global data[42]: %d\n", global_data[42]);
    printf("Temporary data[25]: %.2f\n", temp_data[25]);
    
    printf("Memory allocation from context successful\n");
}

/* Example function that uses the scratch pool */
void scratch_pool_example(myrtx_context_t* ctx) {
    printf("\n=== Scratch Pool Example ===\n");
    
    printf("Using first scratch arena...\n");
    MYRTX_WITH_CONTEXT_SCRATCH(ctx, scratch1) {
        /* Allocate from the first scratch arena */
        char* data1 = (char*)myrtx_arena_alloc(scratch1.arena, 128);
        sprintf(data1, "Data from scratch arena 1");
        printf("scratch1 data: %s\n", data1);
        
        /* Nested scratch arena */
        printf("Using nested scratch arena...\n");
        MYRTX_WITH_CONTEXT_SCRATCH(ctx, scratch2) {
            /* Allocate from the second scratch arena */
            char* data2 = (char*)myrtx_arena_alloc(scratch2.arena, 128);
            sprintf(data2, "Data from scratch arena 2");
            printf("scratch2 data: %s\n", data2);
            
            /* Both data1 and data2 are accessible here */
            printf("Both arenas accessible: %s, %s\n", data1, data2);
        }
        printf("Nested scratch arena released\n");
        
        /* data1 is still accessible, data2 is not */
        printf("After nested scratch: %s\n", data1);
    }
    printf("First scratch arena released\n");
    
    /* Demonstrate scratch arena reuse */
    printf("\nDemonstrating scratch arena reuse...\n");
    
    for (int i = 0; i < 10; i++) {
        printf("Iteration %d: ", i);
        MYRTX_WITH_CONTEXT_SCRATCH(ctx, scratch) {
            /* Allocate memory in the scratch arena */
            void* mem = myrtx_arena_alloc(scratch.arena, 1024);
            printf("allocated at %p\n", mem);
        }
    }
    
    printf("Note how the same address is often reused - this is the pool in action\n");
}

/* Example function that uses custom extensions */
void extension_example(myrtx_context_t* ctx, int extension_id) {
    printf("\n=== Extension Example ===\n");
    
    /* Get our custom extension */
    custom_extension_t* ext = (custom_extension_t*)myrtx_get_extension_data(ctx, extension_id);
    if (!ext) {
        printf("Failed to get custom extension\n");
        return;
    }
    
    /* Use the extension */
    ext->name = strdup("Example Extension");
    ext->count = 5;
    
    for (int i = 0; i < ext->count; i++) {
        ext->values[i] = (float)i * 3.14f;
    }
    
    /* Display extension data */
    printf("Extension name: %s\n", ext->name);
    printf("Extension count: %d\n", ext->count);
    printf("Extension values: ");
    for (int i = 0; i < ext->count; i++) {
        printf("%.2f ", ext->values[i]);
    }
    printf("\n");
}

/* Example function that uses error handling */
void error_handling_example(myrtx_context_t* ctx) {
    printf("\n=== Error Handling Example ===\n");
    
    /* Set an error */
    myrtx_context_set_error(ctx, 42, "An example error occurred: %s", "invalid operation");
    
    /* Get the error */
    const char* error_msg = myrtx_context_get_error(ctx);
    int error_code = myrtx_context_get_error_code(ctx);
    
    printf("Error code: %d\n", error_code);
    printf("Error message: %s\n", error_msg);
}

/* Example of thread-local context */
void thread_local_context_example(myrtx_context_t* ctx) {
    printf("\n=== Thread Local Context Example ===\n");
    
    /* Set the current context */
    myrtx_set_current_context(ctx);
    
    /* Get the current context */
    myrtx_context_t* current = myrtx_get_current_context();
    
    if (current == ctx) {
        printf("Successfully retrieved thread-local context\n");
        
        /* Allocate using the current context */
        void* mem = myrtx_context_alloc(current, 256);
        if (mem) {
            printf("Allocated memory from thread-local context\n");
        }
    } else {
        printf("Failed to retrieve thread-local context\n");
    }
}

int main(void) {
    printf("=== Context System Example ===\n");
    
    /* Register our custom extension */
    int extension_id = register_custom_extension();
    if (extension_id < 0) {
        printf("Failed to register custom extension\n");
        return 1;
    }
    
    printf("Custom extension registered with ID: %d\n", extension_id);
    
    /* Create a context */
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        printf("Failed to create context\n");
        return 1;
    }
    
    printf("Context created successfully\n");
    
    /* Run examples */
    context_allocation_example(ctx);
    scratch_pool_example(ctx);
    extension_example(ctx, extension_id);
    error_handling_example(ctx);
    thread_local_context_example(ctx);
    
    /* Destroy the context */
    myrtx_context_destroy(ctx);
    
    printf("\nExample completed successfully\n");
    return 0;
} 