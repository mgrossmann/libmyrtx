#include "myrtx/memory/arena_allocator.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Private helper functions */

/**
 * @brief Aligns an address to the specified alignment
 */
static inline uintptr_t align_forward(uintptr_t addr, size_t alignment) {
    assert((alignment & (alignment - 1)) == 0); /* Check if alignment is a power of 2 */
    return (addr + (alignment - 1)) & ~(alignment - 1);
}

/**
 * @brief Adds a new block to an arena
 */
static myrtx_arena_block_t* arena_add_block(myrtx_arena_t* arena, size_t min_size) {
    /* Calculate the size of the new block (at least min_size) */
    size_t block_size = arena->block_size;
    if (block_size < min_size) {
        block_size = min_size;
    }
    
    /* Allocate block structure and data memory */
    myrtx_arena_block_t* block = (myrtx_arena_block_t*)malloc(sizeof(myrtx_arena_block_t));
    if (!block) {
        return NULL;
    }
    
    block->base = (uint8_t*)malloc(block_size);
    if (!block->base) {
        free(block);
        return NULL;
    }
    
    /* Initialize block */
    block->next = NULL;
    block->size = block_size;
    block->used = 0;
    
    /* Add block size to statistics */
    arena->total_allocated += block_size;
    
    /* Add block to arena chain */
    if (!arena->first) {
        arena->first = block;
    }
    
    if (arena->current) {
        arena->current->next = block;
    }
    
    arena->current = block;
    
    return block;
}

/* Public API implementation */

bool myrtx_arena_init(myrtx_arena_t* arena, size_t block_size) {
    if (!arena) {
        return false;
    }
    
    /* Reset arena */
    memset(arena, 0, sizeof(myrtx_arena_t));
    
    /* Set block size (or use default value) */
    arena->block_size = block_size > 0 ? block_size : MYRTX_ARENA_DEFAULT_SIZE;
    
    /* Create first block */
    myrtx_arena_block_t* block = arena_add_block(arena, 0);
    if (!block) {
        return false;
    }
    
    return true;
}

void myrtx_arena_free(myrtx_arena_t* arena) {
    if (!arena) {
        return;
    }
    
    /* Free all blocks */
    myrtx_arena_block_t* block = arena->first;
    while (block) {
        myrtx_arena_block_t* next = block->next;
        free(block->base);
        free(block);
        block = next;
    }
    
    /* Reset arena */
    memset(arena, 0, sizeof(myrtx_arena_t));
}

void* myrtx_arena_alloc_aligned(myrtx_arena_t* arena, size_t size, size_t alignment) {
    if (!arena || !size) {
        return NULL;
    }
    
    myrtx_arena_block_t* block = arena->current;
    
    /* If the current block doesn't have sufficient capacity, find a block with enough space */
    uintptr_t current_ptr = (uintptr_t)block->base + block->used;
    uintptr_t aligned_ptr = align_forward(current_ptr, alignment);
    size_t padding = aligned_ptr - current_ptr;
    
    if (block->used + padding + size > block->size) {
        /* Create a new block that's large enough for the request */
        block = arena_add_block(arena, size + alignment - 1);
        if (!block) {
            return NULL;
        }
        
        /* Calculate new pointer */
        current_ptr = (uintptr_t)block->base;
        aligned_ptr = align_forward(current_ptr, alignment);
        padding = aligned_ptr - current_ptr;
    }
    
    /* Update the amount of used memory */
    block->used += padding + size;
    
    /* Return aligned pointer */
    return (void*)aligned_ptr;
}

void* myrtx_arena_alloc(myrtx_arena_t* arena, size_t size) {
    return myrtx_arena_alloc_aligned(arena, size, MYRTX_ARENA_ALIGNMENT);
}

void* myrtx_arena_calloc(myrtx_arena_t* arena, size_t size) {
    void* memory = myrtx_arena_alloc(arena, size);
    if (memory) {
        memset(memory, 0, size);
    }
    return memory;
}

void myrtx_arena_reset(myrtx_arena_t* arena) {
    if (!arena) {
        return;
    }
    
    /* Reset all temporary markers */
    arena->temp_count = 0;
    
    /* Reset used bytes in each block */
    myrtx_arena_block_t* block = arena->first;
    while (block) {
        block->used = 0;
        block = block->next;
    }
    
    /* Reset current block to the first */
    arena->current = arena->first;
}

size_t myrtx_arena_temp_begin(myrtx_arena_t* arena) {
    if (!arena || arena->temp_count >= MYRTX_ARENA_MAX_TEMP_MARKERS) {
        return (size_t)-1;
    }
    
    /* Mark the current state */
    size_t marker = arena->temp_count;
    
    /* Calculate sum of used bytes in all blocks */
    size_t total_used = 0;
    myrtx_arena_block_t* block = arena->first;
    myrtx_arena_block_t* current = NULL;
    
    while (block) {
        total_used += block->used;
        if (block == arena->current) {
            current = block;
        }
        block = block->next;
    }
    
    /* Store relative position of current block */
    size_t current_pos = (current) ? (uintptr_t)current - (uintptr_t)arena->first : 0;
    
    /* Store marker information (encoded as combined value) */
    arena->temp_markers[arena->temp_count++] = (total_used << 16) | (current_pos & 0xFFFF);
    
    return marker;
}

void myrtx_arena_temp_end(myrtx_arena_t* arena, size_t marker) {
    if (!arena || marker >= arena->temp_count || marker >= MYRTX_ARENA_MAX_TEMP_MARKERS) {
        return;
    }
    
    /* Decode marker information */
    size_t marker_data = arena->temp_markers[marker];
    size_t total_used = marker_data >> 16;
    size_t current_pos = marker_data & 0xFFFF;
    
    /* Reset arena to marked state */
    myrtx_arena_block_t* block = arena->first;
    size_t remaining = total_used;
    
    while (block && remaining > 0) {
        if (remaining > block->size) {
            block->used = block->size;
            remaining -= block->size;
        } else {
            block->used = remaining;
            remaining = 0;
        }
        
        /* Reset the following blocks */
        myrtx_arena_block_t* next = block->next;
        while (next) {
            next->used = 0;
            next = next->next;
        }
        
        block = block->next;
    }
    
    /* Restore current block */
    if (current_pos > 0) {
        arena->current = (myrtx_arena_block_t*)((uintptr_t)arena->first + current_pos);
    } else {
        arena->current = arena->first;
    }
    
    /* Remove all subsequent markers */
    arena->temp_count = marker;
}

bool myrtx_scratch_begin(myrtx_scratch_arena_t* scratch, myrtx_arena_t* arena) {
    if (!scratch || !arena) {
        return false;
    }
    
    /* Set a temporary marker in the parent arena */
    size_t marker = myrtx_arena_temp_begin(arena);
    if (marker == (size_t)-1) {
        return false;
    }
    
    scratch->arena = arena;
    scratch->marker = marker;
    
    return true;
}

void myrtx_scratch_end(myrtx_scratch_arena_t* scratch) {
    if (!scratch || !scratch->arena) {
        return;
    }
    
    /* Reset the arena to the temporary marker */
    myrtx_arena_temp_end(scratch->arena, scratch->marker);
    
    /* Reset the scratch arena */
    scratch->arena = NULL;
    scratch->marker = (size_t)-1;
}

void myrtx_arena_stats(myrtx_arena_t* arena, size_t* total_size, size_t* used_size, size_t* block_count) {
    if (!arena) {
        if (total_size) *total_size = 0;
        if (used_size) *used_size = 0;
        if (block_count) *block_count = 0;
        return;
    }
    
    size_t total = 0;
    size_t used = 0;
    size_t count = 0;
    
    myrtx_arena_block_t* block = arena->first;
    while (block) {
        total += block->size;
        used += block->used;
        count++;
        block = block->next;
    }
    
    if (total_size) *total_size = total;
    if (used_size) *used_size = used;
    if (block_count) *block_count = count;
} 