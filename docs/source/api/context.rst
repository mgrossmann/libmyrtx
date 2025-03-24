Context API
==========

The context module provides a flexible context management system for state management and error handling in C applications.

Context Management
----------------

The context system allows state to be tracked and propagated throughout a program execution, with support for error handling and extension data.

Types
~~~~~

.. c:type:: myrtx_context_t

   Opaque structure representing a context.

   .. code-block:: c

      typedef struct myrtx_context {
          // Internal details
      } myrtx_context_t;

.. c:type:: myrtx_extension_info_t

   Structure for describing context extensions.

   .. code-block:: c

      typedef struct myrtx_extension_info {
          const char* name;        // Extension name
          size_t data_size;        // Size of extension data
          void (*init_func)(void*);    // Initialization function
          void (*free_func)(void*);    // Cleanup function
      } myrtx_extension_info_t;

Creation and Destruction
~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: myrtx_context_t* myrtx_context_create(void)

   Creates a new context.

   :return: Pointer to a new context or NULL on error

.. c:function:: myrtx_context_t* myrtx_context_create_child(myrtx_context_t* parent)

   Creates a new child context from a parent context.

   :param parent: Pointer to the parent context
   :return: Pointer to the new child context or NULL on error

.. c:function:: void myrtx_context_free(myrtx_context_t* ctx)

   Frees a context and all associated resources.

   :param ctx: Pointer to the context to free

Error Handling
~~~~~~~~~~~~

.. c:function:: bool myrtx_context_has_error(const myrtx_context_t* ctx)

   Checks if a context has an error.

   :param ctx: Pointer to the context
   :return: true if the context has an error, false otherwise

.. c:function:: const char* myrtx_context_get_error(const myrtx_context_t* ctx)

   Gets the error message from a context.

   :param ctx: Pointer to the context
   :return: Pointer to the error message or NULL if there is no error

.. c:function:: void myrtx_context_set_error(myrtx_context_t* ctx, const char* format, ...)

   Sets an error message on a context.

   :param ctx: Pointer to the context
   :param format: Format string for the error message (printf-style)
   :param ...: Additional arguments for the format string

.. c:function:: void myrtx_context_clear_error(myrtx_context_t* ctx)

   Clears the error state of a context.

   :param ctx: Pointer to the context

Extension Management
~~~~~~~~~~~~~~~~~

.. c:function:: bool myrtx_context_register_extension(const myrtx_extension_info_t* info, int* id_out)

   Registers a new extension type with the context system.

   :param info: Pointer to the extension information
   :param id_out: Pointer to store the extension ID
   :return: true on success, false on error

.. c:function:: void* myrtx_context_get_extension(myrtx_context_t* ctx, int extension_id)

   Gets the extension data for a specified extension ID.

   :param ctx: Pointer to the context
   :param extension_id: ID of the extension
   :return: Pointer to the extension data or NULL if not found

State Management
~~~~~~~~~~~~~

.. c:function:: bool myrtx_context_set_value(myrtx_context_t* ctx, const char* key, void* value)

   Associates a value with a key in the context.

   :param ctx: Pointer to the context
   :param key: Key to associate with the value
   :param value: Pointer to the value
   :return: true on success, false on error

.. c:function:: void* myrtx_context_get_value(const myrtx_context_t* ctx, const char* key)

   Retrieves a value associated with a key from the context.

   :param ctx: Pointer to the context
   :param key: Key to look up
   :return: Pointer to the value or NULL if not found

.. c:function:: bool myrtx_context_remove_value(myrtx_context_t* ctx, const char* key)

   Removes a key-value pair from the context.

   :param ctx: Pointer to the context
   :param key: Key to remove
   :return: true if the key was found and removed, false otherwise

Inheritance and Propagation
~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: myrtx_context_t* myrtx_context_get_parent(const myrtx_context_t* ctx)

   Gets the parent context of a context.

   :param ctx: Pointer to the context
   :return: Pointer to the parent context or NULL if there is no parent

.. c:function:: bool myrtx_context_propagate_error(myrtx_context_t* ctx)

   Propagates an error up the context hierarchy.

   :param ctx: Pointer to the context
   :return: true if an error was propagated, false otherwise 