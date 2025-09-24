#include "myrtx/context/context.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

/* Thread-local storage for current context */
#ifdef _WIN32
__declspec(thread) static myrtx_context_t* current_context = NULL;
#else
static __thread myrtx_context_t* current_context = NULL;
#endif

/* Extension registry */
static myrtx_extension_info_t extension_registry[MYRTX_MAX_EXTENSION_TYPES];
static int extension_count = 0;

/* Initialize a scratch pool */
static void scratch_pool_init(myrtx_scratch_pool_t* pool, myrtx_arena_t* arena) {
    assert(pool != NULL);
    
    pool->parent_arena = arena;
    pool->count = 0;
    memset(pool->arenas, 0, sizeof(pool->arenas));
}

/* Get a scratch arena from the pool or create a new one */
static bool scratch_pool_get(myrtx_scratch_pool_t* pool, myrtx_scratch_arena_t* scratch) {
    assert(pool != NULL);
    assert(scratch != NULL);
    
    if (pool->count > 0) {
        /* Get a scratch arena from the pool */
        pool->count--;
        *scratch = pool->arenas[pool->count];
        
        /* Reset the scratch arena to its initial state */
        myrtx_arena_temp_end(scratch->arena, scratch->marker);
        scratch->marker = myrtx_arena_temp_begin(scratch->arena);
        
        return true;
    } else {
        /* Create a new scratch arena */
        return myrtx_scratch_begin(scratch, pool->parent_arena);
    }
}

/* Return a scratch arena to the pool */
static void scratch_pool_return(myrtx_scratch_pool_t* pool, myrtx_scratch_arena_t* scratch) {
    assert(pool != NULL);
    assert(scratch != NULL);
    
    if (pool->count < MYRTX_MAX_SCRATCH_POOL_SIZE) {
        /* Add the scratch arena to the pool */
        pool->arenas[pool->count] = *scratch;
        pool->count++;
    } else {
        /* Pool is full, just end the scratch arena */
        myrtx_scratch_end(scratch);
    }
}

myrtx_context_t* myrtx_context_create(myrtx_arena_t* global_arena) {
    /* Allocate context structure */
    myrtx_context_t* context = (myrtx_context_t*)malloc(sizeof(myrtx_context_t));
    if (!context) {
        return NULL;
    }
    
    /* Initialize context */
    memset(context, 0, sizeof(myrtx_context_t));
    
    /* Set up the global arena */
    if (global_arena) {
        context->global_arena = global_arena;
        context->owns_global_arena = false;
    } else {
        context->global_arena = (myrtx_arena_t*)malloc(sizeof(myrtx_arena_t));
        if (!context->global_arena) {
            free(context);
            return NULL;
        }
        
        if (!myrtx_arena_init(context->global_arena, 0)) {
            free(context->global_arena);
            free(context);
            return NULL;
        }
        context->owns_global_arena = true;
    }
    
    /* Set up the temporary arena */
    context->temp_arena = (myrtx_arena_t*)malloc(sizeof(myrtx_arena_t));
    if (!context->temp_arena) {
        if (global_arena == NULL) {
            myrtx_arena_free(context->global_arena);
            free(context->global_arena);
        }
        free(context);
        return NULL;
    }
    
    if (!myrtx_arena_init(context->temp_arena, 0)) {
        free(context->temp_arena);
        if (global_arena == NULL) {
            myrtx_arena_free(context->global_arena);
            free(context->global_arena);
        }
        free(context);
        return NULL;
    }
    
    /* Initialize the scratch pool */
    scratch_pool_init(&context->scratch_pool, context->temp_arena);
    
    /* Initialize extensions */
    for (int i = 0; i < extension_count; i++) {
        if (extension_registry[i].data_size > 0) {
            context->extension_data[i] = calloc(1, extension_registry[i].data_size);
            
            if (context->extension_data[i] && extension_registry[i].initialize) {
                extension_registry[i].initialize(context->extension_data[i]);
            }
        }
    }
    
    return context;
}

void myrtx_context_destroy(myrtx_context_t* context) {
    if (!context) {
        return;
    }
    
    /* If this is the current context, clear it */
    if (current_context == context) {
        current_context = NULL;
    }
    
    /* Finalize extensions */
    for (int i = 0; i < extension_count; i++) {
        if (context->extension_data[i] && extension_registry[i].finalize) {
            extension_registry[i].finalize(context->extension_data[i]);
            free(context->extension_data[i]);
        }
    }
    
    /* Free arenas */
    myrtx_arena_free(context->temp_arena);
    free(context->temp_arena);
    
    /* Only free the global arena if we own it */
    if (context->owns_global_arena && context->global_arena) {
        myrtx_arena_free(context->global_arena);
        free(context->global_arena);
    }
    
    /* Free context structure */
    free(context);
}

int myrtx_register_extension(const myrtx_extension_info_t* info) {
    if (!info || extension_count >= MYRTX_MAX_EXTENSION_TYPES) {
        return -1;
    }
    
    int id = extension_count;
    extension_registry[id] = *info;
    extension_count++;
    
    return id;
}

void* myrtx_get_extension_data(myrtx_context_t* context, int type_id) {
    if (!context || type_id < 0 || type_id >= extension_count) {
        return NULL;
    }
    
    return context->extension_data[type_id];
}

void myrtx_set_current_context(myrtx_context_t* context) {
    current_context = context;
}

myrtx_context_t* myrtx_get_current_context(void) {
    return current_context;
}

void* myrtx_context_alloc(myrtx_context_t* context, size_t size) {
    if (!context || !context->global_arena) {
        return NULL;
    }
    
    return myrtx_arena_alloc(context->global_arena, size);
}

void* myrtx_context_temp_alloc(myrtx_context_t* context, size_t size) {
    if (!context || !context->temp_arena) {
        return NULL;
    }
    
    return myrtx_arena_alloc(context->temp_arena, size);
}

bool myrtx_context_scratch_begin(myrtx_context_t* context, myrtx_scratch_arena_t* scratch) {
    if (!context || !scratch) {
        return false;
    }
    
    return scratch_pool_get(&context->scratch_pool, scratch);
}

void myrtx_context_scratch_end(myrtx_context_t* context, myrtx_scratch_arena_t* scratch) {
    if (!context || !scratch) {
        return;
    }
    
    scratch_pool_return(&context->scratch_pool, scratch);
}

void myrtx_context_set_error(myrtx_context_t* context, int error_code, const char* format, ...) {
    if (!context || !format) {
        return;
    }
    
    context->error_code = error_code;
    
    va_list args;
    va_start(args, format);
    vsnprintf(context->error_buffer, sizeof(context->error_buffer), format, args);
    va_end(args);
}

const char* myrtx_context_get_error(myrtx_context_t* context) {
    if (!context) {
        return "Invalid context";
    }
    
    return context->error_buffer;
}

int myrtx_context_get_error_code(myrtx_context_t* context) {
    if (!context) {
        return -1;
    }
    
    return context->error_code;
} 
