/**
 * @file context_test.c
 * @brief Tests for the myrtx Context System
 */

#include "myrtx/context/context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_PASSED() printf("PASSED: %s\n", __func__)
#define TEST_FAILED(msg) do { printf("FAILED: %s - %s\n", __func__, msg); exit(1); } while(0)

/* Test extension data */
typedef struct {
    int value;
    char* data;
} test_extension_t;

/* Extension callbacks */
static void test_extension_init(void* data) {
    test_extension_t* ext = (test_extension_t*)data;
    ext->value = 0;
    ext->data = NULL;
}

static void test_extension_finalize(void* data) {
    test_extension_t* ext = (test_extension_t*)data;
    if (ext->data) {
        free(ext->data);
        ext->data = NULL;
    }
}

/* Register test extension */
static int register_test_extension(void) {
    myrtx_extension_info_t info = {
        .name = "test_extension",
        .data_size = sizeof(test_extension_t),
        .initialize = test_extension_init,
        .finalize = test_extension_finalize
    };
    
    return myrtx_register_extension(&info);
}

/* Test context creation and destruction */
void test_context_create_destroy(void) {
    /* Create context */
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Verify context was initialized */
    if (!ctx->global_arena || !ctx->temp_arena) {
        TEST_FAILED("Context arenas were not initialized");
    }
    
    /* Check scratch pool initialization */
    if (ctx->scratch_pool.count != 0 || ctx->scratch_pool.parent_arena != ctx->temp_arena) {
        TEST_FAILED("Scratch pool was not initialized correctly");
    }
    
    /* Destroy context */
    myrtx_context_destroy(ctx);
    
    TEST_PASSED();
}

/* Test context memory allocation */
void test_context_memory(void) {
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Allocate from global arena */
    const size_t global_size = 1024;
    void* global_mem = myrtx_context_alloc(ctx, global_size);
    if (!global_mem) {
        TEST_FAILED("Failed to allocate from global arena");
    }
    
    /* Write to memory to ensure it's valid */
    memset(global_mem, 0xAA, global_size);
    
    /* Allocate from temporary arena */
    const size_t temp_size = 512;
    void* temp_mem = myrtx_context_temp_alloc(ctx, temp_size);
    if (!temp_mem) {
        TEST_FAILED("Failed to allocate from temporary arena");
    }
    
    /* Write to memory to ensure it's valid */
    memset(temp_mem, 0xBB, temp_size);
    
    /* Verify the memory contents */
    unsigned char* global_ptr = (unsigned char*)global_mem;
    unsigned char* temp_ptr = (unsigned char*)temp_mem;
    
    for (size_t i = 0; i < global_size; i++) {
        if (global_ptr[i] != 0xAA) {
            TEST_FAILED("Global memory was corrupted");
        }
    }
    
    for (size_t i = 0; i < temp_size; i++) {
        if (temp_ptr[i] != 0xBB) {
            TEST_FAILED("Temporary memory was corrupted");
        }
    }
    
    myrtx_context_destroy(ctx);
    
    TEST_PASSED();
}

/* Test scratch pool functionality */
void test_context_scratch_pool(void) {
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Create multiple scratch arenas to test the pool */
    void* addresses[MYRTX_MAX_SCRATCH_POOL_SIZE * 2];
    
    /* First pass - create scratch arenas */
    for (int i = 0; i < MYRTX_MAX_SCRATCH_POOL_SIZE; i++) {
        myrtx_scratch_arena_t scratch = {0};
        if (!myrtx_context_scratch_begin(ctx, &scratch)) {
            TEST_FAILED("Failed to begin scratch arena");
        }
        
        /* Allocate memory in the scratch arena */
        addresses[i] = myrtx_arena_alloc(scratch.arena, 256);
        if (!addresses[i]) {
            TEST_FAILED("Failed to allocate from scratch arena");
        }
        
        /* End scratch arena, which adds it to the pool */
        myrtx_context_scratch_end(ctx, &scratch);
    }
    
    /* Verify that scratch arenas were created */
    if (ctx->scratch_pool.count == 0) {
        TEST_FAILED("No scratch arenas were added to the pool");
    }
    
    /* Second pass - should reuse the scratch arenas from the pool */
    for (int i = 0; i < MYRTX_MAX_SCRATCH_POOL_SIZE; i++) {
        myrtx_scratch_arena_t scratch = {0};
        if (!myrtx_context_scratch_begin(ctx, &scratch)) {
            TEST_FAILED("Failed to begin scratch arena in second pass");
        }
        
        /* Allocate from the scratch arena */
        addresses[i + MYRTX_MAX_SCRATCH_POOL_SIZE] = myrtx_arena_alloc(scratch.arena, 256);
        if (!addresses[i + MYRTX_MAX_SCRATCH_POOL_SIZE]) {
            TEST_FAILED("Failed to allocate from scratch arena in second pass");
        }
        
        /* End scratch arena */
        myrtx_context_scratch_end(ctx, &scratch);
    }
    
    /* Verify that at least some addresses are reused */
    int address_matches = 0;
    for (int i = 0; i < MYRTX_MAX_SCRATCH_POOL_SIZE; i++) {
        for (int j = 0; j < MYRTX_MAX_SCRATCH_POOL_SIZE; j++) {
            if (addresses[i] == addresses[j + MYRTX_MAX_SCRATCH_POOL_SIZE]) {
                address_matches++;
                break;
            }
        }
    }
    
    if (address_matches == 0) {
        TEST_FAILED("No scratch arenas were reused");
    }
    
    myrtx_context_destroy(ctx);
    
    TEST_PASSED();
}

/* Test the MYRTX_WITH_CONTEXT_SCRATCH macro */
void test_context_scratch_macro(void) {
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Use the macro */
    int found_data = 0;
    MYRTX_WITH_CONTEXT_SCRATCH(ctx, scratch1) {
        /* Allocate and use memory */
        int* data = myrtx_arena_alloc(scratch1.arena, 16 * sizeof(int));
        if (!data) {
            TEST_FAILED("Failed to allocate from scratch arena");
        }
        
        /* Initialize the data */
        for (int i = 0; i < 16; i++) {
            data[i] = i * 2;
        }
        
        /* Check the data */
        for (int i = 0; i < 16; i++) {
            if (data[i] != i * 2) {
                TEST_FAILED("Data was not initialized correctly");
            }
        }
        
        found_data = 1;
    }
    
    if (!found_data) {
        TEST_FAILED("Scratch macro code block was not executed");
    }
    
    /* Test early return with the macro */
    int reached_end = 0;
    MYRTX_WITH_CONTEXT_SCRATCH(ctx, scratch2) {
        /* This is enough to verify it works */
        if (1) {
            /* Early return */
            goto early_exit;
        }
        
        /* This should not be executed */
        reached_end = 1;
    }
early_exit:
    
    if (reached_end) {
        TEST_FAILED("Early exit did not work with scratch macro");
    }
    
    myrtx_context_destroy(ctx);
    
    TEST_PASSED();
}

/* Test extension system */
void test_context_extensions(void) {
    /* Register extension type */
    int ext_id = register_test_extension();
    if (ext_id < 0) {
        TEST_FAILED("Failed to register extension");
    }
    
    /* Create context with extension */
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Get extension data */
    test_extension_t* ext = myrtx_get_extension_data(ctx, ext_id);
    if (!ext) {
        TEST_FAILED("Failed to get extension data");
    }
    
    /* Verify initial state set by initializer */
    if (ext->value != 0 || ext->data != NULL) {
        TEST_FAILED("Extension was not initialized correctly");
    }
    
    /* Modify extension data */
    ext->value = 42;
    ext->data = strdup("Test extension data");
    if (!ext->data) {
        TEST_FAILED("Failed to allocate extension data");
    }
    
    /* Verify the data */
    if (ext->value != 42 || strcmp(ext->data, "Test extension data") != 0) {
        TEST_FAILED("Extension data was not set correctly");
    }
    
    /* Destroy context - should call finalizer */
    myrtx_context_destroy(ctx);
    
    TEST_PASSED();
}

/* Test error handling */
void test_context_error_handling(void) {
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Set an error */
    const int test_code = 42;
    const char* test_msg = "Test error message";
    myrtx_context_set_error(ctx, test_code, "%s", test_msg);
    
    /* Get the error */
    const char* error_msg = myrtx_context_get_error(ctx);
    int error_code = myrtx_context_get_error_code(ctx);
    
    /* Verify error was set correctly */
    if (error_code != test_code) {
        TEST_FAILED("Error code was not set correctly");
    }
    
    if (strcmp(error_msg, test_msg) != 0) {
        TEST_FAILED("Error message was not set correctly");
    }
    
    /* Set a formatted error */
    myrtx_context_set_error(ctx, 100, "Error %d: %s", 100, "Formatted error");
    
    /* Get the error */
    error_msg = myrtx_context_get_error(ctx);
    error_code = myrtx_context_get_error_code(ctx);
    
    /* Verify error was set correctly */
    if (error_code != 100) {
        TEST_FAILED("Formatted error code was not set correctly");
    }
    
    if (strcmp(error_msg, "Error 100: Formatted error") != 0) {
        TEST_FAILED("Formatted error message was not set correctly");
    }
    
    myrtx_context_destroy(ctx);
    
    TEST_PASSED();
}

/* Test thread-local context */
void test_context_thread_local(void) {
    /* Create context */
    myrtx_context_t* ctx = myrtx_context_create(NULL);
    if (!ctx) {
        TEST_FAILED("Failed to create context");
    }
    
    /* Initially, there should be no current context */
    if (myrtx_get_current_context() != NULL) {
        TEST_FAILED("Current context should be NULL initially");
    }
    
    /* Set current context */
    myrtx_set_current_context(ctx);
    
    /* Get current context */
    myrtx_context_t* current = myrtx_get_current_context();
    
    /* Verify current context was set correctly */
    if (current != ctx) {
        TEST_FAILED("Current context was not set correctly");
    }
    
    /* Clean up */
    myrtx_set_current_context(NULL);
    myrtx_context_destroy(ctx);
    
    /* Verify current context was cleared */
    if (myrtx_get_current_context() != NULL) {
        TEST_FAILED("Current context was not cleared");
    }
    
    TEST_PASSED();
}

int main(void) {
    printf("=== myrtx Context System Tests ===\n\n");
    
    test_context_create_destroy();
    test_context_memory();
    test_context_scratch_pool();
    test_context_scratch_macro();
    test_context_extensions();
    test_context_error_handling();
    test_context_thread_local();
    
    printf("\nAll context tests successful!\n");
    return 0;
} 