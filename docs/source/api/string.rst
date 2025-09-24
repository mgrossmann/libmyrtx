String API
=========

Overview
--------

The String API provides utilities for string manipulation, formatting, and management in the libmyrtx library. It offers functions for common string operations with memory safety and performance considerations.

Types
-----

.. c:type:: myrtx_string_t

   An opaque structure representing a dynamic string. This string structure provides automatic memory management and safe string manipulation.

.. c:type:: myrtx_string_view_t

   A structure representing a non-owning view into a string or memory region. String views are lightweight references that don't own or manage memory.

   .. code-block:: c

      typedef struct {
          const char* data;  /* Pointer to the string data */
          size_t length;     /* Length of the string (excluding null terminator) */
      } myrtx_string_view_t;

Creation and Destruction
-----------------------

.. c:function:: myrtx_string_t* myrtx_string_create(void)

   Creates a new empty string.
   
   :return: A pointer to a new string, or NULL on allocation failure.

.. c:function:: myrtx_string_t* myrtx_string_create_from_cstr(const char* cstr)

   Creates a new string from a null-terminated C string.
   
   :param cstr: The source C string.
   :return: A pointer to a new string, or NULL on allocation failure.

.. c:function:: myrtx_string_t* myrtx_string_create_from_buffer(const char* buffer, size_t length)

   Creates a new string from a buffer with specified length.
   
   :param buffer: The source buffer.
   :param length: The number of bytes to copy from the buffer.
   :return: A pointer to a new string, or NULL on allocation failure.

.. c:function:: myrtx_string_t* myrtx_string_create_from_string(const myrtx_string_t* other)

   Creates a new string as a copy of another string.
   
   :param other: The source string to copy.
   :return: A pointer to a new string, or NULL on allocation failure.

.. c:function:: myrtx_string_t* myrtx_string_create_from_arena(myrtx_arena_t* arena)

   Creates a new string that will use the specified arena for allocations.
   
   :param arena: The arena to use for allocations.
   :return: A pointer to a new string, or NULL on error.

.. c:function:: void myrtx_string_free(myrtx_string_t* string)

   Frees a string and its associated memory.
   
   :param string: The string to free.

String Operations
---------------

.. c:function:: int myrtx_string_append(myrtx_string_t* string, const char* buffer, size_t length)

   Appends a buffer to a string.
   
   :param string: The target string.
   :param buffer: The buffer to append.
   :param length: The number of bytes to append from the buffer.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_append_cstr(myrtx_string_t* string, const char* cstr)

   Appends a null-terminated C string to a string.
   
   :param string: The target string.
   :param cstr: The C string to append.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_append_string(myrtx_string_t* string, const myrtx_string_t* other)

   Appends another string to a string.
   
   :param string: The target string.
   :param other: The string to append.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_append_char(myrtx_string_t* string, char c)

   Appends a single character to a string.
   
   :param string: The target string.
   :param c: The character to append.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_append_format(myrtx_string_t* string, const char* format, ...)

   Appends a formatted string to a string.
   
   :param string: The target string.
   :param format: The format string.
   :param ...: The format arguments.
   :return: 0 on success, negative value on error.

Replace
-------

.. c:function:: bool myrtx_string_replace(myrtx_string_t* string, const char* old_str, const char* new_str)

   Replaces all occurrences of ``old_str`` with ``new_str``.

   - For strings backed by an arena, the replacement allocates a fresh buffer
     sized exactly to the result. After replacement, ``capacity == length + 1``.
   - For malloc-backed strings, capacity may be larger due to growth strategy.

   :param string: Target string to modify in place
   :param old_str: Substring to be replaced (must be non-empty)
   :param new_str: Replacement substring
   :return: true on success, false on allocation error

.. c:function:: int myrtx_string_append_format_va(myrtx_string_t* string, const char* format, va_list args)

   Appends a formatted string to a string using a va_list.
   
   :param string: The target string.
   :param format: The format string.
   :param args: The format arguments as a va_list.
   :return: 0 on success, negative value on error.

String Views
-----------

.. c:function:: myrtx_string_view_t myrtx_string_view_from_cstr(const char* cstr)

   Creates a string view from a null-terminated C string.
   
   :param cstr: The source C string.
   :return: A string view.

.. c:function:: myrtx_string_view_t myrtx_string_view_from_buffer(const char* buffer, size_t length)

   Creates a string view from a buffer with specified length.
   
   :param buffer: The source buffer.
   :param length: The length of the buffer.
   :return: A string view.

.. c:function:: myrtx_string_view_t myrtx_string_view_from_string(const myrtx_string_t* string)

   Creates a string view from a string.
   
   :param string: The source string.
   :return: A string view.

String Access
-----------

.. c:function:: const char* myrtx_string_cstr(const myrtx_string_t* string)

   Gets the C string representation of a string.
   
   :param string: The string.
   :return: The null-terminated C string.

.. c:function:: size_t myrtx_string_length(const myrtx_string_t* string)

   Gets the length of a string.
   
   :param string: The string.
   :return: The length of the string (excluding null terminator).

.. c:function:: bool myrtx_string_is_empty(const myrtx_string_t* string)

   Checks if a string is empty.
   
   :param string: The string.
   :return: True if the string is empty, false otherwise.

String Modification
-----------------

.. c:function:: int myrtx_string_clear(myrtx_string_t* string)

   Clears a string, setting its length to 0.
   
   :param string: The string to clear.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_resize(myrtx_string_t* string, size_t new_length)

   Resizes a string to the specified length.
   
   :param string: The string to resize.
   :param new_length: The new length.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_reserve(myrtx_string_t* string, size_t capacity)

   Reserves memory for a string.
   
   :param string: The string.
   :param capacity: The number of bytes to reserve.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_shrink_to_fit(myrtx_string_t* string)

   Shrinks a string's capacity to fit its content.
   
   :param string: The string.
   :return: 0 on success, negative value on error.

String Searching
--------------

.. c:function:: size_t myrtx_string_find(const myrtx_string_t* string, const char* substring, size_t start_pos)

   Finds the first occurrence of a substring in a string, starting from a specified position.
   
   :param string: The string to search in.
   :param substring: The substring to find.
   :param start_pos: The position to start searching from.
   :return: The position of the first occurrence, or SIZE_MAX if not found.

.. c:function:: size_t myrtx_string_rfind(const myrtx_string_t* string, const char* substring, size_t start_pos)

   Finds the last occurrence of a substring in a string, starting from a specified position.
   
   :param string: The string to search in.
   :param substring: The substring to find.
   :param start_pos: The position to start searching from (moving backward).
   :return: The position of the last occurrence, or SIZE_MAX if not found.

String Comparison
---------------

.. c:function:: int myrtx_string_compare(const myrtx_string_t* lhs, const myrtx_string_t* rhs)

   Compares two strings lexicographically.
   
   :param lhs: The first string.
   :param rhs: The second string.
   :return: 0 if equal, negative if lhs < rhs, positive if lhs > rhs.

.. c:function:: int myrtx_string_compare_cstr(const myrtx_string_t* string, const char* cstr)

   Compares a string with a C string lexicographically.
   
   :param string: The string.
   :param cstr: The C string.
   :return: 0 if equal, negative if string < cstr, positive if string > cstr.

.. c:function:: bool myrtx_string_equals(const myrtx_string_t* lhs, const myrtx_string_t* rhs)

   Checks if two strings are equal.
   
   :param lhs: The first string.
   :param rhs: The second string.
   :return: True if the strings are equal, false otherwise.

.. c:function:: bool myrtx_string_equals_cstr(const myrtx_string_t* string, const char* cstr)

   Checks if a string is equal to a C string.
   
   :param string: The string.
   :param cstr: The C string.
   :return: True if the string is equal to the C string, false otherwise.

String Modification
-----------------

.. c:function:: int myrtx_string_substr(const myrtx_string_t* string, size_t pos, size_t length, myrtx_string_t* result)

   Extracts a substring from a string.
   
   :param string: The source string.
   :param pos: The starting position.
   :param length: The length of the substring.
   :param result: The string to store the substring in.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_replace(myrtx_string_t* string, size_t pos, size_t length, const char* replacement, size_t replacement_length)

   Replaces a portion of a string with another string.
   
   :param string: The string to modify.
   :param pos: The position to start replacing at.
   :param length: The length of the portion to replace.
   :param replacement: The replacement string.
   :param replacement_length: The length of the replacement string.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_insert(myrtx_string_t* string, size_t pos, const char* buffer, size_t length)

   Inserts a buffer into a string at the specified position.
   
   :param string: The string to modify.
   :param pos: The position to insert at.
   :param buffer: The buffer to insert.
   :param length: The length of the buffer.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_erase(myrtx_string_t* string, size_t pos, size_t length)

   Erases a portion of a string.
   
   :param string: The string to modify.
   :param pos: The position to start erasing at.
   :param length: The length of the portion to erase.
   :return: 0 on success, negative value on error.

String Utility Functions
----------------------

.. c:function:: myrtx_string_t* myrtx_string_format(const char* format, ...)

   Creates a new string from a format string.
   
   :param format: The format string.
   :param ...: The format arguments.
   :return: A pointer to a new string, or NULL on error.

.. c:function:: myrtx_string_t* myrtx_string_format_va(const char* format, va_list args)

   Creates a new string from a format string using a va_list.
   
   :param format: The format string.
   :param args: The format arguments.
   :return: A pointer to a new string, or NULL on error.

.. c:function:: int myrtx_string_to_lower(myrtx_string_t* string)

   Converts a string to lowercase.
   
   :param string: The string to convert.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_to_upper(myrtx_string_t* string)

   Converts a string to uppercase.
   
   :param string: The string to convert.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_trim(myrtx_string_t* string)

   Trims whitespace from the beginning and end of a string.
   
   :param string: The string to trim.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_trim_left(myrtx_string_t* string)

   Trims whitespace from the beginning of a string.
   
   :param string: The string to trim.
   :return: 0 on success, negative value on error.

.. c:function:: int myrtx_string_trim_right(myrtx_string_t* string)

   Trims whitespace from the end of a string.
   
   :param string: The string to trim.
   :return: 0 on success, negative value on error. 
