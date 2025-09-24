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
    
    /* Initialize block to zero first */
    memset(block, 0, sizeof(myrtx_arena_block_t));
    
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
        if (block->base) {
            free(block->base);
            block->base = NULL;
        }
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
    
    /* Keep the first block but free all others */
    if (arena->first) {
        myrtx_arena_block_t* to_keep = arena->first;
        myrtx_arena_block_t* to_free = to_keep->next;
        
        /* Reset the first block */
        to_keep->used = 0;
        to_keep->next = NULL;
        
        /* Free all other blocks */
        while (to_free) {
            myrtx_arena_block_t* next = to_free->next;
            free(to_free->base);
            free(to_free);
            to_free = next;
        }
        
        /* Reset the current block to the first block */
        arena->current = arena->first;
        
        /* Update total allocated memory */
        arena->total_allocated = arena->first->size;
    }
}

size_t myrtx_arena_temp_begin(myrtx_arena_t* arena) {
    if (!arena || arena->temp_count >= MYRTX_ARENA_MAX_TEMP_MARKERS) {
        return (size_t)-1;
    }

    size_t marker = arena->temp_count;
    
    /* Snapshot the current block and its used offset */
    myrtx_arena_marker_t m;
    m.block = arena->current;
    m.used = (arena->current) ? arena->current->used : 0;
    arena->temp_markers[arena->temp_count++] = m;

    return marker;
}

void myrtx_arena_temp_end(myrtx_arena_t* arena, size_t marker) {
    if (!arena || marker >= arena->temp_count || marker >= MYRTX_ARENA_MAX_TEMP_MARKERS) {
        return;
    }

    myrtx_arena_marker_t m = arena->temp_markers[marker];
    myrtx_arena_block_t* target = m.block;

    if (!target) {
        /* Nothing to restore; should not happen for a valid arena */
        arena->temp_count = marker; /* drop this and later markers */
        return;
    }

    /* Restore used of the target block */
    if (target->used > m.used) {
        target->used = m.used;
    } else {
        /* If used somehow shrank or stayed same, still ensure to set to snapshot value */
        target->used = m.used;
    }

    /* Free blocks allocated after the target block and update total_allocated */
    myrtx_arena_block_t* to_free = target->next;
    target->next = NULL;
    while (to_free) {
        myrtx_arena_block_t* next = to_free->next;
        if (to_free->base) {
            free(to_free->base);
        }
        arena->total_allocated -= to_free->size;
        free(to_free);
        to_free = next;
    }

    /* Set current block back to the target */
    arena->current = target;

    /* Drop this and all later markers */
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
