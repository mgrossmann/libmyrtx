Context System Example
====================

Overview
--------

This example demonstrates how to use the Context System for error handling and state management in a practical application. We'll implement a simple configuration parser that reads settings from a file using the Context System to handle errors and maintain state.

Example Code
-----------

.. code-block:: c

    /**
     * @file context_example.c
     * @brief Example demonstrating the libmyrtx Context System.
     */
    
    #include <myrtx/context.h>
    #include <myrtx/memory.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    
    /* Configuration structure to hold parsed settings */
    typedef struct {
        char* server_address;
        int server_port;
        bool verbose_logging;
        int connection_timeout;
    } config_t;
    
    /**
     * @brief Parse a single configuration line.
     * 
     * @param ctx Context for error handling
     * @param line The configuration line to parse
     * @param config The configuration structure to update
     * @return true if parsed successfully, false otherwise
     */
    bool parse_config_line(myrtx_context_t* ctx, const char* line, config_t* config) {
        /* Create a child context for this specific operation */
        myrtx_context_t* line_ctx = myrtx_context_create_child(ctx);
        if (!line_ctx) {
            myrtx_context_set_error(ctx, "Failed to create child context");
            return false;
        }
    
        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') {
            myrtx_context_free(line_ctx);
            return true;
        }
    
        /* Find the equals sign separating key and value */
        const char* equals = strchr(line, '=');
        if (!equals) {
            myrtx_context_set_error(line_ctx, "Invalid config line (missing '='): %s", line);
            myrtx_context_propagate_error(line_ctx);
            myrtx_context_free(line_ctx);
            return false;
        }
    
        /* Extract the key and value */
        size_t key_len = equals - line;
        char* key = malloc(key_len + 1);
        if (!key) {
            myrtx_context_set_error(line_ctx, "Memory allocation failed");
            myrtx_context_propagate_error(line_ctx);
            myrtx_context_free(line_ctx);
            return false;
        }
    
        strncpy(key, line, key_len);
        key[key_len] = '\0';
    
        /* Trim whitespace from key */
        char* trimmed_key = key;
        while (*trimmed_key && *trimmed_key == ' ') trimmed_key++;
        
        char* end = trimmed_key + strlen(trimmed_key) - 1;
        while (end > trimmed_key && *end == ' ') {
            *end = '\0';
            end--;
        }
    
        /* Extract value */
        const char* value = equals + 1;
        /* Skip leading whitespace in value */
        while (*value && *value == ' ') value++;
    
        /* Process the key-value pair */
        if (strcmp(trimmed_key, "server_address") == 0) {
            config->server_address = strdup(value);
            if (!config->server_address) {
                myrtx_context_set_error(line_ctx, "Memory allocation failed for server_address");
                free(key);
                myrtx_context_propagate_error(line_ctx);
                myrtx_context_free(line_ctx);
                return false;
            }
        } else if (strcmp(trimmed_key, "server_port") == 0) {
            char* endptr;
            long port = strtol(value, &endptr, 10);
            
            if (*endptr != '\0' || port < 1 || port > 65535) {
                myrtx_context_set_error(line_ctx, "Invalid server port: %s", value);
                free(key);
                myrtx_context_propagate_error(line_ctx);
                myrtx_context_free(line_ctx);
                return false;
            }
            
            config->server_port = (int)port;
        } else if (strcmp(trimmed_key, "verbose_logging") == 0) {
            if (strcmp(value, "true") == 0 || strcmp(value, "yes") == 0 || strcmp(value, "1") == 0) {
                config->verbose_logging = true;
            } else if (strcmp(value, "false") == 0 || strcmp(value, "no") == 0 || strcmp(value, "0") == 0) {
                config->verbose_logging = false;
            } else {
                myrtx_context_set_error(line_ctx, "Invalid verbose_logging value: %s", value);
                free(key);
                myrtx_context_propagate_error(line_ctx);
                myrtx_context_free(line_ctx);
                return false;
            }
        } else if (strcmp(trimmed_key, "connection_timeout") == 0) {
            char* endptr;
            long timeout = strtol(value, &endptr, 10);
            
            if (*endptr != '\0' || timeout < 0) {
                myrtx_context_set_error(line_ctx, "Invalid connection timeout: %s", value);
                free(key);
                myrtx_context_propagate_error(line_ctx);
                myrtx_context_free(line_ctx);
                return false;
            }
            
            config->connection_timeout = (int)timeout;
        } else {
            myrtx_context_set_error(line_ctx, "Unknown configuration key: %s", trimmed_key);
            free(key);
            myrtx_context_propagate_error(line_ctx);
            myrtx_context_free(line_ctx);
            return false;
        }
    
        free(key);
        myrtx_context_free(line_ctx);
        return true;
    }
    
    /**
     * @brief Load configuration from a file.
     * 
     * @param ctx Context for error handling
     * @param filename The configuration file to load
     * @param config The configuration structure to populate
     * @return true if loaded successfully, false otherwise
     */
    bool load_config(myrtx_context_t* ctx, const char* filename, config_t* config) {
        FILE* file = fopen(filename, "r");
        if (!file) {
            myrtx_context_set_error(ctx, "Failed to open config file: %s", filename);
            return false;
        }
        
        /* Associate the file with the context so we can close it if there's an error */
        myrtx_context_set_value(ctx, "config_file", file);
        
        /* Set default values */
        config->server_address = NULL;
        config->server_port = 8080;
        config->verbose_logging = false;
        config->connection_timeout = 30;
        
        char line[256];
        int line_number = 0;
        
        while (fgets(line, sizeof(line), file)) {
            line_number++;
            
            /* Remove newline character */
            size_t len = strlen(line);
            if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
                line[len-1] = '\0';
            }
            
            /* Create a child context for each line, including line number for context */
            myrtx_context_t* line_ctx = myrtx_context_create_child(ctx);
            if (!line_ctx) {
                myrtx_context_set_error(ctx, "Failed to create context for line %d", line_number);
                fclose(file);
                return false;
            }
            
            /* Store line number in context */
            int* line_num_ptr = malloc(sizeof(int));
            if (!line_num_ptr) {
                myrtx_context_set_error(ctx, "Memory allocation failed");
                myrtx_context_free(line_ctx);
                fclose(file);
                return false;
            }
            
            *line_num_ptr = line_number;
            myrtx_context_set_value(line_ctx, "line_number", line_num_ptr);
            
            if (!parse_config_line(line_ctx, line, config)) {
                /* If there was an error, add line number context to the error message */
                const char* error = myrtx_context_get_error(line_ctx);
                if (error) {
                    myrtx_context_set_error(ctx, "Line %d: %s", line_number, error);
                }
                
                /* Clean up */
                int* line_num = myrtx_context_get_value(line_ctx, "line_number");
                if (line_num) free(line_num);
                
                myrtx_context_free(line_ctx);
                fclose(file);
                return false;
            }
            
            /* Clean up line context */
            int* line_num = myrtx_context_get_value(line_ctx, "line_number");
            if (line_num) free(line_num);
            
            myrtx_context_free(line_ctx);
        }
        
        fclose(file);
        return true;
    }
    
    /**
     * @brief Free resources associated with a configuration.
     * 
     * @param config The configuration to free
     */
    void free_config(config_t* config) {
        if (config->server_address) {
            free(config->server_address);
            config->server_address = NULL;
        }
    }
    
    /**
     * @brief Display the configuration values.
     * 
     * @param config The configuration to display
     */
    void display_config(const config_t* config) {
        printf("Configuration:\n");
        printf("  Server Address: %s\n", config->server_address ? config->server_address : "(not set)");
        printf("  Server Port: %d\n", config->server_port);
        printf("  Verbose Logging: %s\n", config->verbose_logging ? "enabled" : "disabled");
        printf("  Connection Timeout: %d seconds\n", config->connection_timeout);
    }
    
    /**
     * @brief Extension for parsing statistics.
     */
    typedef struct {
        int lines_parsed;
        int errors_encountered;
        int warnings_generated;
    } parser_stats_t;
    
    void parser_stats_init(void* data) {
        parser_stats_t* stats = (parser_stats_t*)data;
        stats->lines_parsed = 0;
        stats->errors_encountered = 0;
        stats->warnings_generated = 0;
    }
    
    /* No dynamic allocations to free */
    void parser_stats_free(void* data) {
        (void)data;  /* Unused parameter */
    }
    
    int parser_stats_extension_id = -1;
    
    int main(int argc, char** argv) {
        /* Register our extension for parser statistics */
        myrtx_extension_info_t stats_info = {
            .name = "parser_stats",
            .data_size = sizeof(parser_stats_t),
            .init_func = parser_stats_init,
            .free_func = parser_stats_free
        };
        
        myrtx_context_register_extension(&stats_info, &parser_stats_extension_id);
        
        /* Create a root context */
        myrtx_context_t* ctx = myrtx_context_create();
        if (!ctx) {
            fprintf(stderr, "Failed to create context\n");
            return 1;
        }
        
        /* Get our parser stats extension */
        parser_stats_t* stats = myrtx_context_get_extension(ctx, parser_stats_extension_id);
        if (!stats) {
            fprintf(stderr, "Failed to get parser stats extension\n");
            myrtx_context_free(ctx);
            return 1;
        }
        
        /* Set up arena allocator for temporary string operations */
        myrtx_arena_t arena;
        if (myrtx_arena_init(&arena, 4096) != 0) {
            fprintf(stderr, "Failed to initialize arena allocator\n");
            myrtx_context_free(ctx);
            return 1;
        }
        
        /* Store arena in context for cleanup later */
        myrtx_context_set_value(ctx, "arena", &arena);
        
        config_t config;
        const char* filename = (argc > 1) ? argv[1] : "config.ini";
        
        printf("Loading configuration from: %s\n", filename);
        
        if (load_config(ctx, filename, &config)) {
            printf("\nConfiguration loaded successfully!\n\n");
            display_config(&config);
            
            /* Display stats */
            printf("\nParser Statistics:\n");
            printf("  Lines Parsed: %d\n", stats->lines_parsed);
            printf("  Errors Encountered: %d\n", stats->errors_encountered);
            printf("  Warnings Generated: %d\n", stats->warnings_generated);
            
            free_config(&config);
        } else {
            fprintf(stderr, "\nFailed to load configuration: %s\n", 
                    myrtx_context_get_error(ctx) ? myrtx_context_get_error(ctx) : "Unknown error");
        }
        
        /* Clean up */
        myrtx_arena_free(&arena);
        myrtx_context_free(ctx);
        
        return 0;
    }

How to Compile and Run
---------------------

To compile this example:

.. code-block:: bash

    gcc -o context_example context_example.c -lmyrtx

Create a sample configuration file `config.ini`:

.. code-block:: ini

    # Server configuration
    server_address = 192.168.1.100
    server_port = 8080
    
    # Logging settings
    verbose_logging = true
    
    # Timeouts
    connection_timeout = 60

Run the example:

.. code-block:: bash

    ./context_example config.ini

Expected Output
-------------

.. code-block:: text

    Loading configuration from: config.ini
    
    Configuration loaded successfully!
    
    Configuration:
      Server Address: 192.168.1.100
      Server Port: 8080
      Verbose Logging: enabled
      Connection Timeout: 60 seconds
    
    Parser Statistics:
      Lines Parsed: 9
      Errors Encountered: 0
      Warnings Generated: 0

If the configuration file contains errors, you'll see detailed error messages, for example:

.. code-block:: text

    Loading configuration from: invalid_config.ini
    
    Failed to load configuration: Line 3: Invalid server port: 99999

Code Walkthrough
--------------

1. **Context Creation and Hierarchy**

   We create a root context in `main()` that serves as the top level for our application:

   .. code-block:: c

       myrtx_context_t* ctx = myrtx_context_create();

   For each operation, like parsing a configuration line, we create a child context:

   .. code-block:: c

       myrtx_context_t* line_ctx = myrtx_context_create_child(ctx);

   This hierarchy allows errors to be properly scoped and propagated up when needed.

2. **Error Handling**

   Throughout the code, we use the context system to set and propagate errors:

   .. code-block:: c

       myrtx_context_set_error(line_ctx, "Invalid server port: %s", value);
       myrtx_context_propagate_error(line_ctx);

   When an error occurs, we include detailed information about what went wrong, and we can add additional context as the error propagates up the call stack.

3. **State Management**

   We use the context to store and retrieve values:

   .. code-block:: c

       myrtx_context_set_value(ctx, "config_file", file);
       myrtx_context_set_value(line_ctx, "line_number", line_num_ptr);

   This allows us to associate important data with each context, making it available to functions that receive the context.

4. **Extensions**

   We use a custom extension to track parsing statistics:

   .. code-block:: c

       myrtx_extension_info_t stats_info = {
           .name = "parser_stats",
           .data_size = sizeof(parser_stats_t),
           .init_func = parser_stats_init,
           .free_func = parser_stats_free
       };
       
       myrtx_context_register_extension(&stats_info, &parser_stats_extension_id);

   And then access it throughout the code:

   .. code-block:: c

       parser_stats_t* stats = myrtx_context_get_extension(ctx, parser_stats_extension_id);

   This provides a clean way to associate custom structured data with contexts.

5. **Resource Management**

   We use the Context System to help manage resources by associating them with contexts and ensuring proper cleanup:

   .. code-block:: c

       myrtx_context_set_value(ctx, "arena", &arena);

   This pattern helps ensure that resources are properly tracked and can be cleaned up, even in error cases.

Key Insights
----------

1. **Structured Error Handling**: The Context System provides a more structured approach to error handling than traditional C error codes. Errors include detailed messages and can be propagated up a call stack.

2. **Hierarchical State**: By creating parent-child relationships between contexts, we can create a natural hierarchy for both error propagation and state inheritance.

3. **Resource Management**: Associating resources with contexts can help ensure proper cleanup, even in complex error cases.

4. **Extensibility**: Custom extensions provide a clean way to associate structured data with contexts, allowing for flexible and maintainable code.

This example demonstrates how the Context System can be used to create cleaner, more maintainable C code with robust error handling and state management. 