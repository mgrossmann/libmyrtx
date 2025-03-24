/**
 * @file arena_example.c
 * @brief Example usage of the myrtx Arena Allocator
 */

#include "myrtx/memory/arena_allocator.h"
#include <stdio.h>
#include <time.h>

/* Structure for test items */
typedef struct test_item {
    int id;
    char data[1024]; /* A large field to simulate memory consumption */
} test_item_t;

void demonstrate_basic_arena(void) {
    printf("=== Basic Arena Usage ===\n");
    
    /* Initialize arena with default block size */
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Error initializing arena!\n");
        return;
    }
    
    /* Allocate some items */
    const int num_items = 1000;
    test_item_t** items = (test_item_t**)myrtx_arena_alloc(&arena, num_items * sizeof(test_item_t*));
    
    for (int i = 0; i < num_items; i++) {
        items[i] = (test_item_t*)myrtx_arena_alloc(&arena, sizeof(test_item_t));
        items[i]->id = i;
    }
    
    printf("Allocated: %d elements with %zu bytes each\n", num_items, sizeof(test_item_t));
    
    /* Display statistics */
    size_t total_size, used_size, block_count;
    myrtx_arena_stats(&arena, &total_size, &used_size, &block_count);
    
    printf("Arena Statistics:\n");
    printf("  Total memory: %zu bytes\n", total_size);
    printf("  Used memory: %zu bytes\n", used_size);
    printf("  Block count: %zu\n", block_count);
    printf("  Memory usage efficiency: %.2f%%\n", (float)used_size / total_size * 100.0f);
    
    /* Reset and free arena */
    myrtx_arena_reset(&arena);
    printf("Arena reset. Memory can be reused.\n");
    
    myrtx_arena_free(&arena);
    printf("Arena freed.\n");
}

void demonstrate_temp_arena(void) {
    printf("\n=== Temporary Arena Usage ===\n");
    
    /* Initialize arena with a smaller block size */
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 64 * 1024)) { /* 64 KB blocks */
        printf("Error initializing arena!\n");
        return;
    }
    
    /* Allocate some items */
    printf("Allocating some items before the temporary region...\n");
    test_item_t* permanent_item = (test_item_t*)myrtx_arena_alloc(&arena, sizeof(test_item_t));
    permanent_item->id = 9999;
    
    /* Begin a temporary region */
    printf("Beginning temporary region...\n");
    size_t temp_marker = myrtx_arena_temp_begin(&arena);
    
    /* Allocate many items in the temporary region */
    printf("Allocating items in the temporary region...\n");
    for (int i = 0; i < 100; i++) {
        test_item_t* temp_item = (test_item_t*)myrtx_arena_alloc(&arena, sizeof(test_item_t));
        temp_item->id = i;
    }
    
    /* Display statistics before resetting */
    size_t total_before, used_before, blocks_before;
    myrtx_arena_stats(&arena, &total_before, &used_before, &blocks_before);
    
    printf("Arena statistics before resetting the temporary region:\n");
    printf("  Total memory: %zu bytes\n", total_before);
    printf("  Used memory: %zu bytes\n", used_before);
    printf("  Block count: %zu\n", blocks_before);
    
    /* End temporary region */
    printf("Ending temporary region...\n");
    myrtx_arena_temp_end(&arena, temp_marker);
    
    /* Display statistics after resetting */
    size_t total_after, used_after, blocks_after;
    myrtx_arena_stats(&arena, &total_after, &used_after, &blocks_after);
    
    printf("Arena statistics after resetting the temporary region:\n");
    printf("  Total memory: %zu bytes\n", total_after);
    printf("  Used memory: %zu bytes\n", used_after);
    printf("  Block count: %zu\n", blocks_after);
    
    /* Check if the permanent item still exists */
    printf("Permanent item ID: %d (should be 9999)\n", permanent_item->id);
    
    /* Free arena */
    myrtx_arena_free(&arena);
}

void demonstrate_scratch_arena(void) {
    printf("\n=== Scratch Arena Usage ===\n");
    
    /* Initialize main arena */
    myrtx_arena_t main_arena = {0};
    if (!myrtx_arena_init(&main_arena, 0)) {
        printf("Error initializing arena!\n");
        return;
    }
    
    /* Use main arena */
    printf("Allocating elements in the main arena...\n");
    test_item_t* main_item = (test_item_t*)myrtx_arena_alloc(&main_arena, sizeof(test_item_t));
    main_item->id = 1;
    
    /* Create and use scratch arena */
    {
        printf("Beginning scratch arena...\n");
        myrtx_scratch_arena_t scratch = {0};
        if (!myrtx_scratch_begin(&scratch, &main_arena)) {
            printf("Error creating scratch arena!\n");
            return;
        }
        
        printf("Allocating elements in the scratch arena...\n");
        for (int i = 0; i < 50; i++) {
            test_item_t* scratch_item = (test_item_t*)myrtx_arena_alloc(scratch.arena, sizeof(test_item_t));
            scratch_item->id = 1000 + i;
        }
        
        /* Display statistics during scratch arena */
        size_t total_during, used_during, blocks_during;
        myrtx_arena_stats(scratch.arena, &total_during, &used_during, &blocks_during);
        
        printf("Arena statistics during scratch arena:\n");
        printf("  Total memory: %zu bytes\n", total_during);
        printf("  Used memory: %zu bytes\n", used_during);
        printf("  Block count: %zu\n", blocks_during);
        
        printf("Ending scratch arena...\n");
        myrtx_scratch_end(&scratch);
    }
    
    /* Display statistics after scratch arena */
    size_t total_after, used_after, blocks_after;
    myrtx_arena_stats(&main_arena, &total_after, &used_after, &blocks_after);
    
    printf("Arena statistics after ending the scratch arena:\n");
    printf("  Total memory: %zu bytes\n", total_after);
    printf("  Used memory: %zu bytes\n", used_after);
    printf("  Block count: %zu\n", blocks_after);
    
    /* Check if the main arena item still exists */
    printf("Main arena item ID: %d (should be 1)\n", main_item->id);
    
    /* Free main arena */
    myrtx_arena_free(&main_arena);
}

void benchmark_arena_vs_malloc(void) {
    printf("\n=== Performance Comparison: Arena vs. malloc/free ===\n");
    
    const int num_allocations = 10000;
    const size_t alloc_size = 128;
    clock_t start, end;
    double time_arena, time_malloc;
    
    /* Test arena */
    myrtx_arena_t arena = {0};
    myrtx_arena_init(&arena, 0);
    
    start = clock();
    for (int i = 0; i < num_allocations; i++) {
        void* mem = myrtx_arena_alloc(&arena, alloc_size);
        /* Use memory to prevent optimization */
        ((char*)mem)[0] = (char)i;
    }
    end = clock();
    time_arena = (double)(end - start) / CLOCKS_PER_SEC;
    
    myrtx_arena_free(&arena);
    
    /* Test malloc/free */
    void* pointers[num_allocations];
    
    start = clock();
    for (int i = 0; i < num_allocations; i++) {
        pointers[i] = malloc(alloc_size);
        /* Use memory to prevent optimization */
        ((char*)pointers[i])[0] = (char)i;
    }
    
    for (int i = 0; i < num_allocations; i++) {
        free(pointers[i]);
    }
    end = clock();
    time_malloc = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* Display results */
    printf("Performance comparison for %d allocations of %zu bytes each:\n", num_allocations, alloc_size);
    printf("  Arena time: %.6f seconds\n", time_arena);
    printf("  malloc/free time: %.6f seconds\n", time_malloc);
    printf("  Speed improvement: %.2fx\n", time_malloc / time_arena);
}

int main(void) {
    printf("=== Arena Allocator Example ===\n\n");
    
    demonstrate_basic_arena();
    demonstrate_temp_arena();
    demonstrate_scratch_arena();
    benchmark_arena_vs_malloc();
    
    printf("\nExample completed.\n");
    return 0;
} 