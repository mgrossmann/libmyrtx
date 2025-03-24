/**
 * @file arena_test.c
 * @brief Tests for the myrtx Arena Allocator
 */

#include "myrtx/memory/arena_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_PASSED() printf("PASSED: %s\n", __func__)
#define TEST_FAILED(msg) do { printf("FAILED: %s - %s\n", __func__, msg); exit(1); } while(0)

void test_arena_init(void) {
    myrtx_arena_t arena = {0};
    
    /* Test with default size */
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena with default size");
    }
    
    size_t total_size, used_size, block_count;
    myrtx_arena_stats(&arena, &total_size, &used_size, &block_count);
    
    if (total_size != MYRTX_ARENA_DEFAULT_SIZE) {
        TEST_FAILED("Arena does not have the correct default size");
    }
    
    if (used_size != 0) {
        TEST_FAILED("Newly initialized arena should have no used bytes");
    }
    
    if (block_count != 1) {
        TEST_FAILED("Newly initialized arena should have exactly one block");
    }
    
    myrtx_arena_free(&arena);
    
    /* Test with custom size */
    const size_t custom_size = 4096;
    if (!myrtx_arena_init(&arena, custom_size)) {
        TEST_FAILED("Could not initialize arena with custom size");
    }
    
    myrtx_arena_stats(&arena, &total_size, &used_size, &block_count);
    
    if (total_size != custom_size) {
        TEST_FAILED("Arena does not have the correct custom size");
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

void test_arena_alloc(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena");
    }
    
    /* Allocations of different sizes */
    const size_t small_size = 16;
    const size_t medium_size = 256;
    const size_t large_size = 1024;
    
    void* small_alloc = myrtx_arena_alloc(&arena, small_size);
    void* medium_alloc = myrtx_arena_alloc(&arena, medium_size);
    void* large_alloc = myrtx_arena_alloc(&arena, large_size);
    
    if (!small_alloc || !medium_alloc || !large_alloc) {
        TEST_FAILED("One of the allocations failed");
    }
    
    /* Test if memory regions overlap */
    uintptr_t small_end = (uintptr_t)small_alloc + small_size;
    uintptr_t medium_end = (uintptr_t)medium_alloc + medium_size;
    
    if (small_end > (uintptr_t)medium_alloc) {
        TEST_FAILED("Memory regions overlap (small and medium)");
    }
    
    if (medium_end > (uintptr_t)large_alloc) {
        TEST_FAILED("Memory regions overlap (medium and large)");
    }
    
    /* Check used bytes */
    size_t total_size, used_size, block_count;
    myrtx_arena_stats(&arena, &total_size, &used_size, &block_count);
    
    /* Note that alignment can lead to more used bytes */
    if (used_size < small_size + medium_size + large_size) {
        TEST_FAILED("Used bytes are smaller than the sum of allocations");
    }
    
    /* Test writing to allocated memory */
    memset(small_alloc, 0xAA, small_size);
    memset(medium_alloc, 0xBB, medium_size);
    memset(large_alloc, 0xCC, large_size);
    
    /* Ensure data was written correctly */
    unsigned char* small_ptr = (unsigned char*)small_alloc;
    unsigned char* medium_ptr = (unsigned char*)medium_alloc;
    unsigned char* large_ptr = (unsigned char*)large_alloc;
    
    for (size_t i = 0; i < small_size; i++) {
        if (small_ptr[i] != 0xAA) {
            TEST_FAILED("Data in small memory region was not written correctly");
        }
    }
    
    for (size_t i = 0; i < medium_size; i++) {
        if (medium_ptr[i] != 0xBB) {
            TEST_FAILED("Data in medium memory region was not written correctly");
        }
    }
    
    for (size_t i = 0; i < large_size; i++) {
        if (large_ptr[i] != 0xCC) {
            TEST_FAILED("Data in large memory region was not written correctly");
        }
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

void test_arena_reset(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena");
    }
    
    /* Perform some allocations */
    for (int i = 0; i < 100; i++) {
        void* ptr = myrtx_arena_alloc(&arena, 128);
        if (!ptr) {
            TEST_FAILED("Allocation failed");
        }
        /* Write to memory to ensure it's valid */
        memset(ptr, i, 128);
    }
    
    /* Statistics before reset */
    size_t total_before, used_before, blocks_before;
    myrtx_arena_stats(&arena, &total_before, &used_before, &blocks_before);
    
    if (used_before < 100 * 128) {
        TEST_FAILED("Not enough memory was used");
    }
    
    /* Reset arena */
    myrtx_arena_reset(&arena);
    
    /* Statistics after reset */
    size_t total_after, used_after, blocks_after;
    myrtx_arena_stats(&arena, &total_after, &used_after, &blocks_after);
    
    if (used_after != 0) {
        TEST_FAILED("Arena was not reset correctly (used_size should be 0)");
    }
    
    if (total_after != total_before) {
        TEST_FAILED("Total size of arena should not change after reset");
    }
    
    if (blocks_after != blocks_before) {
        TEST_FAILED("Number of blocks should not change after reset");
    }
    
    /* Perform new allocations after reset */
    for (int i = 0; i < 100; i++) {
        void* ptr = myrtx_arena_alloc(&arena, 128);
        if (!ptr) {
            TEST_FAILED("Allocation after reset failed");
        }
        /* Write to memory to ensure it's valid */
        memset(ptr, i, 128);
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

void test_arena_temp(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena");
    }
    
    /* Allocation before temporary region */
    void* ptr_before = myrtx_arena_alloc(&arena, 512);
    if (!ptr_before) {
        TEST_FAILED("Allocation before temporary region failed");
    }
    memset(ptr_before, 0xDD, 512);
    
    /* Statistics before temporary region */
    size_t used_before;
    myrtx_arena_stats(&arena, NULL, &used_before, NULL);
    
    /* Set temporary marker */
    size_t marker = myrtx_arena_temp_begin(&arena);
    if (marker == (size_t)-1) {
        TEST_FAILED("Could not set temporary marker");
    }
    
    /* Allocations in temporary region */
    for (int i = 0; i < 50; i++) {
        void* temp_ptr = myrtx_arena_alloc(&arena, 256);
        if (!temp_ptr) {
            TEST_FAILED("Allocation in temporary region failed");
        }
        memset(temp_ptr, i, 256);
    }
    
    /* Statistics during temporary region */
    size_t used_during;
    myrtx_arena_stats(&arena, NULL, &used_during, NULL);
    
    if (used_during <= used_before) {
        TEST_FAILED("Used bytes should be greater during temporary region");
    }
    
    /* End temporary region */
    myrtx_arena_temp_end(&arena, marker);
    
    /* Statistics after temporary region */
    size_t used_after;
    myrtx_arena_stats(&arena, NULL, &used_after, NULL);
    
    if (used_after != used_before) {
        TEST_FAILED("Used bytes should be equal to the state before temporary region");
    }
    
    /* Check if data before temporary region is still valid */
    unsigned char* check_ptr = (unsigned char*)ptr_before;
    for (size_t i = 0; i < 512; i++) {
        if (check_ptr[i] != 0xDD) {
            TEST_FAILED("Data before temporary region was corrupted");
        }
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

void test_arena_scratch(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena");
    }
    
    /* Allocation in main arena */
    void* main_ptr = myrtx_arena_alloc(&arena, 1024);
    if (!main_ptr) {
        TEST_FAILED("Allocation in main arena failed");
    }
    memset(main_ptr, 0xEE, 1024);
    
    /* Statistics before scratch arena */
    size_t used_before;
    myrtx_arena_stats(&arena, NULL, &used_before, NULL);
    
    /* Create and use scratch arena */
    {
        myrtx_scratch_arena_t scratch = {0};
        if (!myrtx_scratch_begin(&scratch, &arena)) {
            TEST_FAILED("Could not create scratch arena");
        }
        
        /* Allocations in scratch arena */
        for (int i = 0; i < 30; i++) {
            void* scratch_ptr = myrtx_arena_alloc(scratch.arena, 128);
            if (!scratch_ptr) {
                TEST_FAILED("Allocation in scratch arena failed");
            }
            memset(scratch_ptr, i, 128);
        }
        
        /* Statistics during scratch arena */
        size_t used_during;
        myrtx_arena_stats(scratch.arena, NULL, &used_during, NULL);
        
        if (used_during <= used_before) {
            TEST_FAILED("Used bytes should be greater during scratch arena");
        }
        
        /* End scratch arena */
        myrtx_scratch_end(&scratch);
    }
    
    /* Statistics after scratch arena */
    size_t used_after;
    myrtx_arena_stats(&arena, NULL, &used_after, NULL);
    
    if (used_after != used_before) {
        TEST_FAILED("Used bytes should be equal to the state before scratch arena");
    }
    
    /* Check if data in main arena is still valid */
    unsigned char* check_ptr = (unsigned char*)main_ptr;
    for (size_t i = 0; i < 1024; i++) {
        if (check_ptr[i] != 0xEE) {
            TEST_FAILED("Data in main arena was corrupted");
        }
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

void test_arena_calloc(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena");
    }
    
    /* Test myrtx_arena_calloc */
    const size_t alloc_size = 1024;
    unsigned char* calloc_ptr = (unsigned char*)myrtx_arena_calloc(&arena, alloc_size);
    
    if (!calloc_ptr) {
        TEST_FAILED("myrtx_arena_calloc failed");
    }
    
    /* Check if memory is filled with zeros */
    for (size_t i = 0; i < alloc_size; i++) {
        if (calloc_ptr[i] != 0) {
            TEST_FAILED("myrtx_arena_calloc did not fill memory with zeros");
        }
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

void test_arena_aligned(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Could not initialize arena");
    }
    
    /* Test different alignments */
    for (size_t alignment = 1; alignment <= 128; alignment *= 2) {
        void* ptr = myrtx_arena_alloc_aligned(&arena, 512, alignment);
        if (!ptr) {
            TEST_FAILED("Aligned allocation failed");
        }
        
        uintptr_t addr = (uintptr_t)ptr;
        if (addr % alignment != 0) {
            char error_msg[128];
            snprintf(error_msg, sizeof(error_msg), 
                     "Address %p is not aligned to %zu", ptr, alignment);
            TEST_FAILED(error_msg);
        }
    }
    
    myrtx_arena_free(&arena);
    
    TEST_PASSED();
}

int main(void) {
    printf("=== myrtx Arena Allocator Tests ===\n\n");
    
    test_arena_init();
    test_arena_alloc();
    test_arena_reset();
    test_arena_temp();
    test_arena_scratch();
    test_arena_calloc();
    test_arena_aligned();
    
    printf("\nAll tests successful!\n");
    return 0;
} 