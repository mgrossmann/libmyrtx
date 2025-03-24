/**
 * @file arena_allocator.h
 * @brief Arena-based memory allocation system
 *
 * Implements an Arena Allocator for efficient memory management
 * with support for temporary arenas and scratch arenas.
 */

#ifndef MYRTX_ARENA_ALLOCATOR_H
#define MYRTX_ARENA_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of temporary markers supported per arena
 */
#define MYRTX_ARENA_MAX_TEMP_MARKERS 32

/**
 * @brief Alignment size for arena allocations (must be a power of 2)
 */
#define MYRTX_ARENA_ALIGNMENT 8

/**
 * @brief Default size for new arenas
 */
#define MYRTX_ARENA_DEFAULT_SIZE (1024 * 1024) /* 1 MB */

/**
 * @brief Structure representing a single memory block in an arena
 */
typedef struct myrtx_arena_block {
    struct myrtx_arena_block* next;   /**< Next block in the arena */
    uint8_t* base;                    /**< Base address of the block */
    size_t size;                      /**< Total size of the block */
    size_t used;                      /**< Used bytes in the block */
} myrtx_arena_block_t;

/**
 * @brief Main structure for the arena allocator
 */
typedef struct myrtx_arena {
    myrtx_arena_block_t* current;            /**< Current block for allocations */
    myrtx_arena_block_t* first;              /**< First block in the arena */
    size_t block_size;                       /**< Default size for new blocks */
    size_t total_allocated;                  /**< Total allocated memory */
    unsigned int temp_count;                 /**< Number of active temporary markers */
    size_t temp_markers[MYRTX_ARENA_MAX_TEMP_MARKERS]; /**< Temporary markers */
} myrtx_arena_t;

/**
 * @brief A scratch arena uses a temporary marker in a parent arena
 * and ensures the memory is reset when leaving scope
 */
typedef struct myrtx_scratch_arena {
    myrtx_arena_t* arena;       /**< Reference to the parent arena */
    size_t marker;              /**< Stored temporary marker */
} myrtx_scratch_arena_t;

/**
 * @brief Initializes a new arena with a specific block size
 *
 * @param arena Pointer to the arena structure to initialize
 * @param block_size Size of blocks to allocate for this arena 
 *                   (0 for default size)
 * @return true on successful initialization
 * @return false on error
 */
bool myrtx_arena_init(myrtx_arena_t* arena, size_t block_size);

/**
 * @brief Frees all memory used by an arena
 *
 * @param arena Pointer to the arena to free
 */
void myrtx_arena_free(myrtx_arena_t* arena);

/**
 * @brief Allocates a memory block from an arena
 *
 * @param arena Pointer to the arena for allocation
 * @param size Required memory size in bytes
 * @return void* Pointer to the allocated memory or NULL on error
 */
void* myrtx_arena_alloc(myrtx_arena_t* arena, size_t size);

/**
 * @brief Allocates an aligned memory block from an arena
 *
 * @param arena Pointer to the arena for allocation
 * @param size Required memory size in bytes
 * @param alignment Alignment (must be a power of 2)
 * @return void* Pointer to the aligned, allocated memory or NULL on error
 */
void* myrtx_arena_alloc_aligned(myrtx_arena_t* arena, size_t size, size_t alignment);

/**
 * @brief Allocates and zeros a memory block from an arena
 *
 * @param arena Pointer to the arena for allocation
 * @param size Required memory size in bytes
 * @return void* Pointer to the allocated, zeroed memory or NULL on error
 */
void* myrtx_arena_calloc(myrtx_arena_t* arena, size_t size);

/**
 * @brief Resets the current state of an arena
 * 
 * This does not free the total memory allocated by the arena,
 * but simply marks it as reusable.
 *
 * @param arena Pointer to the arena to reset
 */
void myrtx_arena_reset(myrtx_arena_t* arena);

/**
 * @brief Marks a temporary state in an arena
 *
 * @param arena Pointer to the arena in which to set a temporary marker
 * @return size_t Marker value for later use with myrtx_arena_reset_to_temp
 *                or (size_t)-1 on error
 */
size_t myrtx_arena_temp_begin(myrtx_arena_t* arena);

/**
 * @brief Resets an arena to a previously marked temporary state
 *
 * @param arena Pointer to the arena
 * @param marker The marker value returned by myrtx_arena_temp_begin
 */
void myrtx_arena_temp_end(myrtx_arena_t* arena, size_t marker);

/**
 * @brief Initializes a scratch arena
 *
 * A scratch arena is a temporary view of a parent arena,
 * secured by a temporary marker. When the scratch arena
 * is exited, the memory it used is automatically freed.
 *
 * @param scratch Pointer to the scratch arena to initialize
 * @param arena Pointer to the parent arena
 * @return true if the scratch arena was successfully initialized
 * @return false on error
 */
bool myrtx_scratch_begin(myrtx_scratch_arena_t* scratch, myrtx_arena_t* arena);

/**
 * @brief Frees the memory of a scratch arena
 *
 * @param scratch Pointer to the scratch arena to free
 */
void myrtx_scratch_end(myrtx_scratch_arena_t* scratch);

/**
 * @brief Returns statistics about arena usage
 *
 * @param arena Pointer to the arena
 * @param total_size Pointer to a variable to receive the total size (can be NULL)
 * @param used_size Pointer to a variable to receive the used size (can be NULL)
 * @param block_count Pointer to a variable to receive the block count (can be NULL)
 */
void myrtx_arena_stats(myrtx_arena_t* arena, size_t* total_size, size_t* used_size, size_t* block_count);

#ifdef __cplusplus
}
#endif

#endif /* MYRTX_ARENA_ALLOCATOR_H */ 