/**
 * @file string.h
 * @brief String manipulation functions and types for myrtx
 *
 * This file provides a set of string manipulation functions and a dedicated string type
 * that are designed to work efficiently with the myrtx memory allocators, particularly
 * the arena allocator. These functions help with common string operations while allowing
 * control over memory allocation strategies.
 */

#ifndef MYRTX_STRING_H
#define MYRTX_STRING_H

#include "myrtx/memory/arena_allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief String type with built-in length and capacity tracking
 */
typedef struct myrtx_string {
    char* data;        /**< Pointer to string data (null-terminated) */
    size_t length;     /**< Length of string in bytes (excluding null terminator) */
    size_t capacity;   /**< Total allocated capacity in bytes (including null terminator) */
    myrtx_arena_t* arena; /**< Arena used for allocation (NULL if using malloc) */
} myrtx_string_t;

/**
 * @brief Create an empty string
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param initial_capacity Initial capacity for the string
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_create(myrtx_arena_t* arena, size_t initial_capacity);

/**
 * @brief Create a string from a C string
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param cstr C string to copy
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_from_cstr(myrtx_arena_t* arena, const char* cstr);

/**
 * @brief Create a string from a memory buffer with explicit length
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param buffer Memory buffer to copy
 * @param length Length of the buffer
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_from_buffer(myrtx_arena_t* arena, const char* buffer, size_t length);

/**
 * @brief Create a formatted string
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param format Format string (printf style)
 * @param ... Format arguments
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_format(myrtx_arena_t* arena, const char* format, ...);

/**
 * @brief Free resources used by a string
 * 
 * If the string was allocated with an arena, this is a no-op unless force is true.
 * 
 * @param str Pointer to the string
 * @param force If true, free even if using an arena
 */
void myrtx_string_free(myrtx_string_t* str, bool force);

/**
 * @brief Get the length of a string
 * 
 * @param str String to measure
 * @return size_t Length of the string
 */
size_t myrtx_string_length(const myrtx_string_t* str);

/**
 * @brief Get the capacity of a string
 * 
 * @param str String to check
 * @return size_t Capacity of the string
 */
size_t myrtx_string_capacity(const myrtx_string_t* str);

/**
 * @brief Get the underlying C string
 * 
 * @param str String to get data from
 * @return const char* Pointer to null-terminated string data
 */
const char* myrtx_string_cstr(const myrtx_string_t* str);

/**
 * @brief Set the content of a string from a C string
 * 
 * @param str String to modify
 * @param cstr C string to copy
 * @return bool true on success, false on failure
 */
bool myrtx_string_set(myrtx_string_t* str, const char* cstr);

/**
 * @brief Set the content of a string from a buffer
 * 
 * @param str String to modify
 * @param buffer Memory buffer to copy
 * @param length Length of the buffer
 * @return bool true on success, false on failure
 */
bool myrtx_string_set_buffer(myrtx_string_t* str, const char* buffer, size_t length);

/**
 * @brief Append a C string to a string
 * 
 * @param str String to append to
 * @param cstr C string to append
 * @return bool true on success, false on failure
 */
bool myrtx_string_append(myrtx_string_t* str, const char* cstr);

/**
 * @brief Append a buffer to a string
 * 
 * @param str String to append to
 * @param buffer Memory buffer to append
 * @param length Length of the buffer
 * @return bool true on success, false on failure
 */
bool myrtx_string_append_buffer(myrtx_string_t* str, const char* buffer, size_t length);

/**
 * @brief Append a formatted string to a string
 * 
 * @param str String to append to
 * @param format Format string (printf style)
 * @param ... Format arguments
 * @return bool true on success, false on failure
 */
bool myrtx_string_append_format(myrtx_string_t* str, const char* format, ...);

/**
 * @brief Compare two strings
 * 
 * @param str1 First string
 * @param str2 Second string
 * @return int Negative if str1 < str2, positive if str1 > str2, 0 if equal
 */
int myrtx_string_compare(const myrtx_string_t* str1, const myrtx_string_t* str2);

/**
 * @brief Check if a string is empty
 * 
 * @param str String to check
 * @return bool true if the string is empty, false otherwise
 */
bool myrtx_string_is_empty(const myrtx_string_t* str);

/**
 * @brief Clear a string (set length to 0)
 * 
 * @param str String to clear
 */
void myrtx_string_clear(myrtx_string_t* str);

/**
 * @brief Get a substring of a string
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param str Source string
 * @param start Starting position
 * @param length Length of the substring to extract
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_substr(myrtx_arena_t* arena, const myrtx_string_t* str, size_t start, size_t length);

/**
 * @brief Make a copy of a string
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param str String to clone
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_clone(myrtx_arena_t* arena, const myrtx_string_t* str);

/**
 * @brief Resize a string to a new capacity
 * 
 * This function will not shrink the string below its current length.
 * 
 * @param str String to resize
 * @param new_capacity New capacity
 * @return bool true on success, false on failure
 */
bool myrtx_string_reserve(myrtx_string_t* str, size_t new_capacity);

/**
 * @brief Trim whitespace from the beginning and end of a string
 * 
 * @param str String to trim
 * @return bool true on success, false on failure
 */
bool myrtx_string_trim(myrtx_string_t* str);

/**
 * @brief Convert a string to uppercase
 * 
 * @param str String to convert
 * @return bool true on success, false on failure
 */
bool myrtx_string_to_upper(myrtx_string_t* str);

/**
 * @brief Convert a string to lowercase
 * 
 * @param str String to convert
 * @return bool true on success, false on failure
 */
bool myrtx_string_to_lower(myrtx_string_t* str);

/**
 * @brief Check if a string starts with a prefix
 * 
 * @param str String to check
 * @param prefix Prefix to look for
 * @return bool true if the string starts with the prefix, false otherwise
 */
bool myrtx_string_starts_with(const myrtx_string_t* str, const char* prefix);

/**
 * @brief Check if a string ends with a suffix
 * 
 * @param str String to check
 * @param suffix Suffix to look for
 * @return bool true if the string ends with the suffix, false otherwise
 */
bool myrtx_string_ends_with(const myrtx_string_t* str, const char* suffix);

/**
 * @brief Replace all occurrences of a substring in a string
 * 
 * @param str String to modify
 * @param old_str Substring to replace
 * @param new_str Replacement string
 * @return bool true on success, false on failure
 */
bool myrtx_string_replace(myrtx_string_t* str, const char* old_str, const char* new_str);

/**
 * @brief Split a string into an array of strings
 * 
 * @param arena Pointer to the arena to allocate from
 * @param str String to split
 * @param delimiter Delimiter string
 * @param count Pointer to variable that will receive the count of strings
 * @return myrtx_string_t* Array of strings or NULL on failure
 */
myrtx_string_t* myrtx_string_split(myrtx_arena_t* arena, const myrtx_string_t* str, const char* delimiter, size_t* count);

/**
 * @brief Join an array of strings with a delimiter
 * 
 * @param arena Pointer to the arena to allocate from, or NULL to use malloc
 * @param strings Array of strings to join
 * @param count Number of strings
 * @param delimiter Delimiter string
 * @return myrtx_string_t New string or {NULL, 0, 0, NULL} on failure
 */
myrtx_string_t myrtx_string_join(myrtx_arena_t* arena, const myrtx_string_t* strings, size_t count, const char* delimiter);

/**
 * @brief Find the first occurrence of a substring in a string
 * 
 * @param str String to search in
 * @param substr Substring to search for
 * @return size_t Position of the substring or SIZE_MAX if not found
 */
size_t myrtx_string_find(const myrtx_string_t* str, const char* substr);

/**
 * @brief Find the first occurrence of a substring in a string, starting from a position
 * 
 * @param str String to search in
 * @param substr Substring to search for
 * @param pos Starting position
 * @return size_t Position of the substring or SIZE_MAX if not found
 */
size_t myrtx_string_find_from(const myrtx_string_t* str, const char* substr, size_t pos);

/**
 * @brief Find the last occurrence of a substring in a string
 * 
 * @param str String to search in
 * @param substr Substring to search for
 * @return size_t Position of the substring or SIZE_MAX if not found
 */
size_t myrtx_string_rfind(const myrtx_string_t* str, const char* substr);

/*
 * Legacy C string functions that work with arena allocators
 * These are kept for backward compatibility
 */

/**
 * @brief Duplicate a string using the arena allocator
 *
 * @param arena Pointer to the arena to allocate from
 * @param str String to duplicate
 * @return char* Duplicated string or NULL on failure
 */
char* myrtx_strdup(myrtx_arena_t* arena, const char* str);

/**
 * @brief Duplicate up to n characters of a string using the arena allocator
 *
 * @param arena Pointer to the arena to allocate from
 * @param str String to duplicate
 * @param n Maximum number of characters to duplicate
 * @return char* Duplicated string or NULL on failure
 */
char* myrtx_strndup(myrtx_arena_t* arena, const char* str, size_t n);

/**
 * @brief Format a string using the arena allocator
 *
 * Similar to sprintf but allocates the required memory from an arena
 *
 * @param arena Pointer to the arena to allocate from
 * @param format Format string (printf style)
 * @param ... Format arguments
 * @return char* Formatted string or NULL on failure
 */
char* myrtx_strfmt(myrtx_arena_t* arena, const char* format, ...);

/**
 * @brief Create a new string by concatenating two strings
 *
 * @param arena Pointer to the arena to allocate from
 * @param str1 First string
 * @param str2 Second string
 * @return char* Concatenated string or NULL on failure
 */
char* myrtx_strcat_dup(myrtx_arena_t* arena, const char* str1, const char* str2);

/**
 * @brief Split a string into tokens
 *
 * This function splits a string into tokens based on a set of delimiter
 * characters. Unlike strtok, it is thread-safe and does not modify the
 * original string.
 *
 * @param arena Pointer to the arena to allocate from
 * @param str String to split
 * @param delimiters String containing delimiter characters
 * @param count Pointer to variable that will receive the token count
 * @return char** Array of token strings or NULL on failure
 */
char** myrtx_strsplit(myrtx_arena_t* arena, const char* str, const char* delimiters, size_t* count);

/**
 * @brief Join strings with a separator
 *
 * @param arena Pointer to the arena to allocate from
 * @param strings Array of strings to join
 * @param count Number of strings in the array
 * @param separator Separator to insert between strings
 * @return char* Joined string or NULL on failure
 */
char* myrtx_strjoin(myrtx_arena_t* arena, const char** strings, size_t count, const char* separator);

/**
 * @brief Create a substring from a string
 *
 * @param arena Pointer to the arena to allocate from
 * @param str Source string
 * @param start Starting position
 * @param length Length of the substring to extract
 * @return char* Substring or NULL on failure
 */
char* myrtx_substr(myrtx_arena_t* arena, const char* str, size_t start, size_t length);

/**
 * @brief Convert a string to uppercase
 *
 * @param arena Pointer to the arena to allocate from
 * @param str String to convert
 * @return char* Uppercase string or NULL on failure
 */
char* myrtx_strtoupper(myrtx_arena_t* arena, const char* str);

/**
 * @brief Convert a string to lowercase
 *
 * @param arena Pointer to the arena to allocate from
 * @param str String to convert
 * @return char* Lowercase string or NULL on failure
 */
char* myrtx_strtolower(myrtx_arena_t* arena, const char* str);

/**
 * @brief Check if a string starts with a prefix
 *
 * @param str String to check
 * @param prefix Prefix to look for
 * @return true if the string starts with the prefix
 * @return false otherwise
 */
bool myrtx_startswith(const char* str, const char* prefix);

/**
 * @brief Check if a string ends with a suffix
 *
 * @param str String to check
 * @param suffix Suffix to look for
 * @return true if the string ends with the suffix
 * @return false otherwise
 */
bool myrtx_endswith(const char* str, const char* suffix);

/**
 * @brief Trim whitespace from the beginning and end of a string
 *
 * @param arena Pointer to the arena to allocate from
 * @param str String to trim
 * @return char* Trimmed string or NULL on failure
 */
char* myrtx_strtrim(myrtx_arena_t* arena, const char* str);

/**
 * @brief Replace all occurrences of a substring within a string
 *
 * @param arena Pointer to the arena to allocate from
 * @param str Original string
 * @param old_str Substring to replace
 * @param new_str Replacement string
 * @return char* String with replacements or NULL on failure
 */
char* myrtx_strreplace(myrtx_arena_t* arena, const char* str, const char* old_str, const char* new_str);

#ifdef __cplusplus
}
#endif

#endif /* MYRTX_STRING_H */ 