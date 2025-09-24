/**
 * @file context.h
 * @brief Context system for managing global application state
 *
 * The context system provides a structured way to manage application state,
 * memory allocations, and resource management. It serves as a "this" pointer
 * for C applications and supports thread-local contexts.
 */

#ifndef MYRTX_CONTEXT_H
#define MYRTX_CONTEXT_H

#include "myrtx/memory/arena_allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of extension types supported
 */
#define MYRTX_MAX_EXTENSION_TYPES 32

/**
 * @brief Maximum number of scratch arenas in a scratch pool
 */
#define MYRTX_MAX_SCRATCH_POOL_SIZE 8

/**
 * @brief Structure to hold scratch arenas for reuse
 */
typedef struct myrtx_scratch_pool {
    myrtx_arena_t* parent_arena;                   /**< Parent arena for all scratch arenas */
    myrtx_scratch_arena_t arenas[MYRTX_MAX_SCRATCH_POOL_SIZE]; /**< Array of scratch arenas */
    size_t count;                                  /**< Number of active scratch arenas */
} myrtx_scratch_pool_t;

/**
 * @brief Extension type information
 */
typedef struct myrtx_extension_info {
    const char* name;                 /**< Name of the extension */
    size_t data_size;                 /**< Size of extension data */
    void (*initialize)(void*);        /**< Initialization function */
    void (*finalize)(void*);          /**< Cleanup function */
} myrtx_extension_info_t;

/**
 * @brief Main context structure
 */
typedef struct myrtx_context {
    myrtx_arena_t* global_arena;      /**< Global arena for long-lived allocations */
    myrtx_arena_t* temp_arena;        /**< Temporary arena for short-lived allocations */
    myrtx_scratch_pool_t scratch_pool; /**< Pool of scratch arenas for reuse */
    bool owns_global_arena;           /**< Whether context owns and should free global_arena */
    
    void* extension_data[MYRTX_MAX_EXTENSION_TYPES]; /**< Extension storage */
    unsigned int flags;               /**< Context flags */
    
    char error_buffer[256];           /**< Error message buffer */
    int error_code;                   /**< Last error code */
} myrtx_context_t;

/**
 * @brief Initialize a new context
 * 
 * @param global_arena Optional global arena to use (NULL to create new)
 * @return myrtx_context_t* New context or NULL on failure
 */
myrtx_context_t* myrtx_context_create(myrtx_arena_t* global_arena);

/**
 * @brief Free resources associated with a context
 * 
 * @param context Context to free
 */
void myrtx_context_destroy(myrtx_context_t* context);

/**
 * @brief Register a new extension type
 * 
 * @param info Extension information
 * @return int Type ID or -1 on failure
 */
int myrtx_register_extension(const myrtx_extension_info_t* info);

/**
 * @brief Get extension data from a context
 * 
 * @param context Context to query
 * @param type_id Extension type ID
 * @return void* Extension data or NULL if not present
 */
void* myrtx_get_extension_data(myrtx_context_t* context, int type_id);

/**
 * @brief Set the thread-local context
 * 
 * @param context Context to set as current
 */
void myrtx_set_current_context(myrtx_context_t* context);

/**
 * @brief Get the current thread-local context
 * 
 * @return myrtx_context_t* Current context or NULL if none set
 */
myrtx_context_t* myrtx_get_current_context(void);

/**
 * @brief Allocate memory from the context's global arena
 * 
 * @param context Context to use
 * @param size Size to allocate
 * @return void* Allocated memory or NULL on failure
 */
void* myrtx_context_alloc(myrtx_context_t* context, size_t size);

/**
 * @brief Allocate temporary memory from the context
 * 
 * @param context Context to use
 * @param size Size to allocate
 * @return void* Allocated memory or NULL on failure
 */
void* myrtx_context_temp_alloc(myrtx_context_t* context, size_t size);

/**
 * @brief Get a scratch arena from the context's pool or create a new one
 * 
 * @param context Context to use
 * @param scratch Scratch arena to initialize
 * @return bool true on success
 */
bool myrtx_context_scratch_begin(myrtx_context_t* context, myrtx_scratch_arena_t* scratch);

/**
 * @brief Return a scratch arena to the context's pool
 * 
 * @param context Context to use
 * @param scratch Scratch arena to return
 */
void myrtx_context_scratch_end(myrtx_context_t* context, myrtx_scratch_arena_t* scratch);

/**
 * @brief Convenience macro for using a scratch arena from a context
 */
#define MYRTX_WITH_CONTEXT_SCRATCH(context, scratch_name) \
    myrtx_scratch_arena_t scratch_name = {0}; \
    if (myrtx_context_scratch_begin(context, &scratch_name)) \
    for (int _once = 1; _once; _once = 0, myrtx_context_scratch_end(context, &scratch_name))

/**
 * @brief Set an error in the context
 * 
 * @param context Context to update
 * @param error_code Error code
 * @param format Printf-style format string
 * @param ... Format arguments
 */
void myrtx_context_set_error(myrtx_context_t* context, int error_code, const char* format, ...);

/**
 * @brief Get the last error message from a context
 * 
 * @param context Context to query
 * @return const char* Error message
 */
const char* myrtx_context_get_error(myrtx_context_t* context);

/**
 * @brief Get the last error code from a context
 * 
 * @param context Context to query
 * @return int Error code
 */
int myrtx_context_get_error_code(myrtx_context_t* context);

#ifdef __cplusplus
}
#endif

#endif /* MYRTX_CONTEXT_H */ 
