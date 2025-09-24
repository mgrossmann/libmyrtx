String Utility Guide
======================

Overview
--------

The String Utility library of libmyrtx provides a robust and user-friendly solution for string processing in C. It overcomes the common limitations of standard C string functions and reduces frequent sources of errors such as buffer overflows and memory leaks.

Why a Special String Library?
------------------------------------

The standard C library provides basic string functions, but has several significant limitations:

1. **No automatic memory management**: The developer must manually manage memory allocation and deallocation.
2. **Limited functionality**: Many useful operations (such as string concatenation) are cumbersome to implement.
3. **Lack of safety**: The standard functions offer little protection against buffer overflows.
4. **Inefficient concatenations**: Repeated concatenations often result in unnecessary copy operations.

The ``myrtx_string_t`` implementation provides solutions to these problems and adds useful functions that simplify string processing in C applications.

Core Concepts
-------------

The String library works with two main concepts:

1. **Dynamic Strings**: ``myrtx_string_t`` is an opaque structure that represents a dynamically growing string with automatic memory management.
2. **String Views**: ``myrtx_string_view_t`` is a lightweight, non-owning view of a string or memory region.

Dynamic Strings
~~~~~~~~~~~~~~~

Dynamic strings automatically manage their own memory. They grow as needed to accommodate content and can efficiently perform operations like appending, inserting, and deleting.

String Views
~~~~~~~~~~~~

String views provide a non-owning view of a string or memory region. They consist of a pointer to the data and a length, and are useful when you want to reference part of a larger string without copying the content.

Basic Usage
-----------

Creating and Freeing Strings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    #include <myrtx/string.h>

    // Create an empty string
    myrtx_string_t* str = myrtx_string_create();
    if (!str) {
        // Error handling
    }

    // Create a string from a C string
    myrtx_string_t* greeting = myrtx_string_create_from_cstr("Hello, World!");
    if (!greeting) {
        // Error handling
    }

    // Don't forget to free the strings when they are no longer needed
    myrtx_string_free(str);
    myrtx_string_free(greeting);

With Arena Allocator
~~~~~~~~~~~~~~~~~~~~

For even more efficient memory management, the String library can be integrated with the libmyrtx Arena Allocator:

.. code-block:: c

    #include <myrtx/string.h>
    #include <myrtx/memory.h>

    // Initialize arena
    myrtx_arena_t arena;
    myrtx_arena_init(&arena, 4096);  // 4KB block size

    // Create a string that uses the arena
    myrtx_string_t* str = myrtx_string_create_from_arena(&arena);

    // ... perform string operations ...

    // Free the arena (also frees all associated strings)
    myrtx_arena_free(&arena);

String Operations
-----------------

Appending Data
~~~~~~~~~~~~~~

.. code-block:: c

    myrtx_string_t* name = myrtx_string_create();

    // Append C string
    myrtx_string_append_cstr(name, "John");

    // Append single character
    myrtx_string_append_char(name, ' ');

    // Append buffer with specified length
    const char* surname = "Smith";
    myrtx_string_append(name, surname, strlen(surname));

    // Append formatted data
    myrtx_string_append_format(name, " (Age: %d)", 30);

    printf("Name: %s\n", myrtx_string_cstr(name));  // "John Smith (Age: 30)"

    myrtx_string_free(name);

String Comparison
~~~~~~~~~~~~~~~~~

.. code-block:: c

    myrtx_string_t* str1 = myrtx_string_create_from_cstr("Apple");
    myrtx_string_t* str2 = myrtx_string_create_from_cstr("Banana");

    // Check if strings are equal
    bool equal = myrtx_string_equals(str1, str2);  // false

    // Lexicographical comparison
    int cmp = myrtx_string_compare(str1, str2);  // Negative, since "Apple" comes before "Banana" lexicographically

    // Comparison with C string
    bool equals_cstr = myrtx_string_equals_cstr(str1, "Apple");  // true

    myrtx_string_free(str1);
    myrtx_string_free(str2);

String Modification
~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    myrtx_string_t* text = myrtx_string_create_from_cstr("This is a sample text.");

    // Replace part of the string
    myrtx_string_replace(text, 8, 1, "an example", 10);  // "This is an example text."

    // Insert text
    myrtx_string_insert(text, 0, "Note: ", 6);  // "Note: This is an example text."

    // Erase part of the string
    myrtx_string_erase(text, 0, 6);  // "This is an example text."

    // Truncate string
    myrtx_string_resize(text, 12);  // "This is an e" (truncated to 12 characters)

    myrtx_string_free(text);

String Searching
~~~~~~~~~~~~~~~~

.. code-block:: c

    myrtx_string_t* haystack = myrtx_string_create_from_cstr("This is a sample text with sample words.");

    // Find first occurrence
    size_t pos = myrtx_string_find(haystack, "sample", 0);  // finds "sample" at position 10

    // Find next occurrence from position
    size_t next_pos = myrtx_string_find(haystack, "sample", pos + 1);  // finds next occurrence at position 28

    // Find last occurrence
    size_t last_pos = myrtx_string_rfind(haystack, "sample", SIZE_MAX);  // finds last occurrence

    myrtx_string_free(haystack);

Using String Views
~~~~~~~~~~~~~~~~~~

String views are useful when you want to reference parts of strings without copying:

.. code-block:: c

    const char* text = "This is a sample text.";

    // Create string view from C string
    myrtx_string_view_t view = myrtx_string_view_from_cstr(text);

    // Create string view from buffer with length
    myrtx_string_view_t partial_view = myrtx_string_view_from_buffer(text + 8, 1);  // "a"

    // Work with the view
    printf("View: %.*s\n", (int)partial_view.length, partial_view.data);  // "a"

    // String views don't need to be freed as they don't own memory

Creating Formatted Strings
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    // Create a new formatted string
    myrtx_string_t* formatted = myrtx_string_format("User: %s, ID: %d", "John", 12345);

    // Or with VA list
    va_list args;
    va_start(args, format);
    myrtx_string_t* formatted_va = myrtx_string_format_va(format, args);
    va_end(args);

    myrtx_string_free(formatted);
    myrtx_string_free(formatted_va);

Advanced Concepts
-----------------

Memory Reservation
~~~~~~~~~~~~~~~~~~

If you know a string will grow to a certain size, you can reserve memory in advance to avoid repeated reallocations:

.. code-block:: c

    myrtx_string_t* large_string = myrtx_string_create();

    // Reserve 1024 bytes
    myrtx_string_reserve(large_string, 1024);

    // Now you can append up to 1024 bytes without reallocation
    for (int i = 0; i < 100; i++) {
        myrtx_string_append_cstr(large_string, "Data ");
    }

    myrtx_string_free(large_string);

Memory Optimization
~~~~~~~~~~~~~~~~~~~

After a string has reached its final size, you can release excess memory:

.. code-block:: c

    myrtx_string_t* str = myrtx_string_create();
    myrtx_string_reserve(str, 1024);  // Reserves 1024 bytes

    // ... perform string operations, e.g., using only 100 bytes ...

    // Reduce memory to the size actually needed
    myrtx_string_shrink_to_fit(str);

    myrtx_string_free(str);

String Extraction
~~~~~~~~~~~~~~~~~

You can extract substrings:

.. code-block:: c

    myrtx_string_t* source = myrtx_string_create_from_cstr("This is a long example text");
    myrtx_string_t* substring = myrtx_string_create();

    // Extract substring (from position 8, 1 character)
    myrtx_string_substr(source, 8, 1, substring);  // substring now contains "a"

    myrtx_string_free(source);
    myrtx_string_free(substring);

Efficiency and Performance
--------------------------

The String library is designed for efficiency:

1. **Dynamic Sizing**: Strings grow exponentially to minimize the number of reallocations.
2. **Capacity Reservation**: You can reserve memory in advance.
3. **Efficient Concatenation**: Multiple concatenations are optimized.
4. **Arena Integration**: With the Arena Allocator, many temporary strings can be efficiently managed.

Best Practices
--------------

1. **Always Free Strings**: Call `myrtx_string_free()` when a string is no longer needed.

2. **Use Arena for Temporary Strings**: Use an arena for temporary strings to simplify memory management.

3. **Reserve Capacity**: If you know the approximate size of a string, reserve memory in advance.

4. **Use String Views for Substrings**: When you only need to reference parts of a string, use string views instead of creating new strings.

5. **Error Checking**: Check the return values of string functions to detect errors.

Migration Example
~~~~~~~~~~~~~~~~~

Here's an example of how to migrate from standard C string processing to the libmyrtx String library:

Before (Standard C):

.. code-block:: c

    char buffer[256];
    char* result = malloc(1024);
    if (!result) return;

    strcpy(buffer, "Hello, ");
    strcat(buffer, username);
    strcat(buffer, "! ");

    sprintf(result, "%sWelcome to %s. You have %d new messages.",
            buffer, app_name, message_count);

    // Use result...

    free(result);

After (with libmyrtx):

.. code-block:: c

    myrtx_string_t* greeting = myrtx_string_create();
    if (!greeting) return;

    myrtx_string_append_cstr(greeting, "Hello, ");
    myrtx_string_append_cstr(greeting, username);
    myrtx_string_append_cstr(greeting, "! ");

    myrtx_string_append_format(greeting, "Welcome to %s. You have %d new messages.",
                              app_name, message_count);

    // Use myrtx_string_cstr(greeting)...

    myrtx_string_free(greeting);

Error Handling
~~~~~~~~~~~~~~

String functions return error codes when operations fail:

.. code-block:: c

    myrtx_string_t* str = myrtx_string_create();
    if (!str) {
        fprintf(stderr, "Error: Could not create string\n");
        return;
    }

    int result = myrtx_string_append_cstr(str, "Data");
    if (result != 0) {
        fprintf(stderr, "Error appending to string: %d\n", result);
        myrtx_string_free(str);
        return;
    }

    myrtx_string_free(str);

Conclusion
------------

The libmyrtx String Utility library provides a robust, efficient, and user-friendly solution for string processing in C applications. By using dynamic strings, automatic memory management, and a comprehensive set of functions, it improves the safety, readability, and maintainability of your code. The integration with the Arena Allocator makes it particularly efficient for complex applications with many temporary strings. 
