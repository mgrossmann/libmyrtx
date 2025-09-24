Context System Guide
=================

Overview
--------

The Context System is a powerful component of the libmyrtx library that provides a structured approach to error handling, state management, and data sharing across function calls. This guide explains how to use the Context System effectively in your applications.

Why Use the Context System?
-------------------------

Traditional error handling in C often involves passing error codes or using global variables, which can lead to several problems:

1. **Error propagation complexity**: Checking and propagating error codes through multiple function calls can be tedious and error-prone.
2. **Limited error information**: Simple error codes provide limited context about what went wrong.
3. **Global state issues**: Global error variables create thread-safety problems and make it difficult to handle multiple operations concurrently.

The Context System solves these problems by:

1. **Structured error handling**: Errors are associated with a specific context, allowing detailed messages and automatic propagation.
2. **Hierarchical state management**: Contexts can form parent-child relationships, creating a natural scope for data and error handling.
3. **Thread safety**: Each operation can have its own context, eliminating global state concerns.
4. **Extensibility**: The extension mechanism allows custom data to be associated with contexts.

Basic Usage
---------

Creating and Managing Contexts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Start by including the header and creating a context:

.. code-block:: c

    #include <myrtx/context/context.h>
    
    int main() {
        // Create a root context (managed global arena)
        myrtx_context_t* ctx = myrtx_context_create(NULL);
        if (!ctx) {
            fprintf(stderr, "Failed to create context\n");
            return 1;
        }
        
        // Use the context...
        
        // Free the context when done
        myrtx_context_destroy(ctx);
        return 0;
    }

For nested operations, you can create child contexts:

.. code-block:: c

    bool process_data(myrtx_context_t* parent_ctx, const char* data) {
        // Create a child context for this operation
        /* Child contexts are not implemented in this API; create a fresh context
           as needed, or pass the parent where appropriate. */
        myrtx_context_t* ctx = myrtx_context_create(NULL);
        if (!ctx) {
            myrtx_context_set_error(parent_ctx, "Failed to create child context");
            return false;
        }
        
        // Process data using the child context...
        
        // Check if an error occurred during processing
        if (myrtx_context_has_error(ctx)) {
            // Propagate error to parent if needed
            myrtx_context_propagate_error(ctx);
            myrtx_context_destroy(ctx);
            return false;
        }
        
        myrtx_context_destroy(ctx);
        return true;
    }

Error Handling
~~~~~~~~~~~~

The Context System provides rich error handling capabilities:

.. code-block:: c

    bool divide(myrtx_context_t* ctx, int a, int b, int* result) {
        if (b == 0) {
            // Set a descriptive error message
            myrtx_context_set_error(ctx, "Division by zero");
            return false;
        }
        
        *result = a / b;
        return true;
    }
    
    void use_division() {
        myrtx_context_t* ctx = myrtx_context_create(NULL);
        int result;
        
        if (!divide(ctx, 10, 0, &result)) {
            // Check and handle the error
            const char* error = myrtx_context_get_error(ctx);
            fprintf(stderr, "Error: %s\n", error ? error : "Unknown error");
        }
        
        // Clear error if you want to reuse the context
        /* Clear error buffer for reuse */
        (void)myrtx_context_get_error(ctx); /* Accessor returns last message */
        
        myrtx_context_destroy(ctx);
    }

State Management
~~~~~~~~~~~~~~

Contexts can store and retrieve arbitrary key-value data:

.. code-block:: c

    void state_example() {
        myrtx_context_t* ctx = myrtx_context_create(NULL);
        
        // Store some data in the context
        int* counter = malloc(sizeof(int));
        *counter = 42;
        
        myrtx_context_set_value(ctx, "counter", counter);
        
        // Later, retrieve the data
        int* retrieved = myrtx_context_get_value(ctx, "counter");
        if (retrieved) {
            printf("Counter value: %d\n", *retrieved);
        }
        
        // Remove and free the data
        if (myrtx_context_remove_value(ctx, "counter")) {
            free(counter);
        }
        
        myrtx_context_free(ctx);
    }

Advanced Features
---------------

Context Hierarchy
~~~~~~~~~~~~~~~

The parent-child relationship between contexts creates a natural hierarchy for error propagation and data inheritance:

.. code-block:: c

    void hierarchy_example() {
        myrtx_context_t* root = myrtx_context_create();
        
        // Set a value in the root context
        const char* app_name = "MyApplication";
        myrtx_context_set_value(root, "app_name", (void*)app_name);
        
        // Create child contexts for different operations
        myrtx_context_t* child1 = myrtx_context_create_child(root);
        myrtx_context_t* child2 = myrtx_context_create_child(root);
        
        // Child contexts can access parent's values
        printf("Child1 app name: %s\n", (const char*)myrtx_context_get_value(child1, "app_name"));
        
        // Set an error in child1
        myrtx_context_set_error(child1, "Operation failed in child1");
        
        // Propagate the error up to the parent
        if (myrtx_context_has_error(child1)) {
            myrtx_context_propagate_error(child1);
        }
        
        // Now the root context also has the error
        if (myrtx_context_has_error(root)) {
            printf("Root error: %s\n", myrtx_context_get_error(root));
        }
        
        myrtx_context_destroy(child1);
        myrtx_context_destroy(child2);
        myrtx_context_destroy(root);
    }

Ownership Example
-----------------

Using an external arena with a context leaves ownership with the caller:

.. code-block:: c

    myrtx_arena_t external = {0};
    myrtx_arena_init(&external, 0);

    // Context references external arena and will NOT free it
    myrtx_context_t* ctx = myrtx_context_create(&external);
    /* ... use ctx ... */
    myrtx_context_destroy(ctx);

    // External arena still valid here
    myrtx_arena_free(&external);

Context Extensions
~~~~~~~~~~~~~~~~

Extensions allow you to associate custom structured data with a context:

.. code-block:: c

    // Define a custom extension structure
    typedef struct {
        int request_id;
        const char* user;
        double start_time;
    } request_info_t;
    
    // Initialize and free functions for our extension
    void request_info_init(void* data) {
        request_info_t* info = (request_info_t*)data;
        info->request_id = 0;
        info->user = NULL;
        info->start_time = 0.0;
    }
    
    void request_info_free(void* data) {
        // Free any dynamically allocated members if needed
    }
    
    // Global extension ID
    static int request_info_extension_id = -1;
    
    // Register the extension once at program startup
    void initialize_extensions() {
        myrtx_extension_info_t info = {
            .name = "request_info",
            .data_size = sizeof(request_info_t),
            .init_func = request_info_init,
            .free_func = request_info_free
        };
        
        myrtx_context_register_extension(&info, &request_info_extension_id);
    }
    
    // Use the extension in request handling
    void handle_request(myrtx_context_t* ctx, int req_id, const char* user) {
        // Get the extension data from the context
        request_info_t* info = myrtx_context_get_extension(ctx, request_info_extension_id);
        if (info) {
            // Set request information
            info->request_id = req_id;
            info->user = user;
            info->start_time = get_current_time();
            
            printf("Processing request %d for user %s\n", info->request_id, info->user);
        }
        
        // Process the request...
    }

Real-world Example: Web Server Request Handling
-------------------------------------------

Here's a more complete example showing how the Context System can be used in a web server to handle requests:

.. code-block:: c

    #include <myrtx/context.h>
    #include <myrtx/memory.h>
    #include <time.h>
    
    // Extension for HTTP request data
    typedef struct {
        char* method;
        char* path;
        char* query;
        char* client_ip;
        time_t timestamp;
    } http_request_ext_t;
    
    // Extension for response data
    typedef struct {
        int status_code;
        myrtx_arena_t arena;  // Arena for response allocations
        char* content_type;
        void* body;
        size_t body_length;
    } http_response_ext_t;
    
    // Extension initialization functions
    void http_request_init(void* data) {
        http_request_ext_t* req = (http_request_ext_t*)data;
        req->method = NULL;
        req->path = NULL;
        req->query = NULL;
        req->client_ip = NULL;
        req->timestamp = time(NULL);
    }
    
    void http_response_init(void* data) {
        http_response_ext_t* resp = (http_response_ext_t*)data;
        resp->status_code = 200;
        myrtx_arena_init(&resp->arena, 4096);  // 4KB block size
        resp->content_type = NULL;
        resp->body = NULL;
        resp->body_length = 0;
    }
    
    void http_request_free(void* data) {
        // No allocations to free (strings are owned by the server)
    }
    
    void http_response_free(void* data) {
        http_response_ext_t* resp = (http_response_ext_t*)data;
        myrtx_arena_free(&resp->arena);
    }
    
    // Global extension IDs
    static int http_request_ext_id = -1;
    static int http_response_ext_id = -1;
    
    // Initialize extensions
    void http_init_extensions() {
        myrtx_extension_info_t req_info = {
            .name = "http_request",
            .data_size = sizeof(http_request_ext_t),
            .init_func = http_request_init,
            .free_func = http_request_free
        };
        
        myrtx_extension_info_t resp_info = {
            .name = "http_response",
            .data_size = sizeof(http_response_ext_t),
            .init_func = http_response_init,
            .free_func = http_response_free
        };
        
        myrtx_context_register_extension(&req_info, &http_request_ext_id);
        myrtx_context_register_extension(&resp_info, &http_response_ext_id);
    }
    
    // Example request handler
    void handle_user_request(myrtx_context_t* ctx, int user_id) {
        // Get the request and response extensions
        http_request_ext_t* req = myrtx_context_get_extension(ctx, http_request_ext_id);
        http_response_ext_t* resp = myrtx_context_get_extension(ctx, http_response_ext_id);
        
        if (!req || !resp) {
            myrtx_context_set_error(ctx, "Missing required extensions");
            return;
        }
        
        printf("Handling request: %s %s from %s\n", req->method, req->path, req->client_ip);
        
        // Create a child context for the database operation
        myrtx_context_t* db_ctx = myrtx_context_create_child(ctx);
        if (!db_ctx) {
            myrtx_context_set_error(ctx, "Failed to create database context");
            resp->status_code = 500;
            return;
        }
        
        // Try to get user data
        user_data_t* user = get_user_by_id(db_ctx, user_id);
        
        // Check for database errors
        if (myrtx_context_has_error(db_ctx)) {
            // Set HTTP 500 error
            resp->status_code = 500;
            // Copy the error message to the parent context
            myrtx_context_propagate_error(db_ctx);
            myrtx_context_free(db_ctx);
            return;
        }
        
        if (!user) {
            // User not found - set 404 error
            resp->status_code = 404;
            myrtx_context_set_error(ctx, "User %d not found", user_id);
            myrtx_context_free(db_ctx);
            return;
        }
        
        // Generate JSON response using the response arena
        const char* json_template = "{"
            "\"id\": %d,"
            "\"name\": \"%s\","
            "\"email\": \"%s\""
            "}";
        
        size_t json_size = snprintf(NULL, 0, json_template, user->id, user->name, user->email);
        char* json = myrtx_arena_alloc(&resp->arena, json_size + 1);
        
        if (json) {
            snprintf(json, json_size + 1, json_template, user->id, user->name, user->email);
            
            // Set response properties
            resp->content_type = "application/json";
            resp->body = json;
            resp->body_length = json_size;
        } else {
            resp->status_code = 500;
            myrtx_context_set_error(ctx, "Failed to allocate response memory");
        }
        
        free_user_data(user);
        myrtx_context_free(db_ctx);
    }

Best Practices
------------

1. **Context Lifetime**: Ensure that each context is properly freed when no longer needed to prevent memory leaks.

2. **Error Propagation**: Remember to propagate errors up the context hierarchy when appropriate, especially when an error in a child context should influence parent operations.

3. **Resource Management**: Use contexts to track and manage resource lifetimes. When you free a context, make sure any associated resources are also cleaned up.

4. **Thread Safety**: The Context System itself is not thread-safe by default. Each thread should have its own context hierarchy, or you should implement appropriate locking mechanisms.

5. **Extension Registration**: Register extensions early in your application's lifecycle, ideally during initialization, as the IDs must be consistent throughout the application.

6. **Consistent Usage**: Adopt a consistent pattern for using contexts throughout your codebase to make error handling and state management predictable.

Conclusion
---------

The Context System provides a powerful framework for error handling, state management, and data propagation in C applications. By leveraging hierarchical contexts, detailed error messages, and extensible state, you can create more robust and maintainable code.

When used effectively, the Context System eliminates many of the error-handling pain points in C programming and provides a structured approach to managing application state and lifetime. 
