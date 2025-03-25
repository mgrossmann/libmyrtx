#include "myrtx/string/string.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

/*
 * New string type implementation
 */

/* Helper function to allocate string memory */
static char* string_alloc(myrtx_arena_t* arena, size_t size) {
    if (arena) {
        return (char*)myrtx_arena_alloc(arena, size);
    } else {
        return (char*)malloc(size);
    }
}

/* Helper function to free string memory */
static void string_free(myrtx_arena_t* arena, void* ptr) {
    if (!arena && ptr) {
        free(ptr);
    }
    /* If using an arena, we don't free individual allocations */
}

/* Helper function to grow a string's capacity */
static bool string_grow(myrtx_string_t* str, size_t min_capacity) {
    /* If capacity is already sufficient, do nothing */
    if (str->capacity >= min_capacity) {
        return true;
    }
    
    /* Calculate new capacity (double or use min_capacity if larger) */
    size_t new_capacity = str->capacity * 2;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    
    /* Allocate new buffer */
    char* new_data;
    if (str->arena) {
        /* With an arena, we always need to allocate new memory */
        new_data = (char*)myrtx_arena_alloc(str->arena, new_capacity);
        if (new_data) {
            /* Copy old data if we had any */
            if (str->data) {
                memcpy(new_data, str->data, str->length + 1); /* +1 for null terminator */
            } else {
                /* Initialize empty string */
                new_data[0] = '\0';
            }
        }
    } else {
        /* With malloc, we can use realloc */
        new_data = (char*)realloc(str->data, new_capacity);
    }
    
    if (!new_data) {
        return false;
    }
    
    str->data = new_data;
    str->capacity = new_capacity;
    return true;
}

/* Helper function to allocate string structure */
static myrtx_string_t* string_struct_alloc(myrtx_arena_t* arena) {
    if (arena) {
        return (myrtx_string_t*)myrtx_arena_alloc(arena, sizeof(myrtx_string_t));
    } else {
        return (myrtx_string_t*)malloc(sizeof(myrtx_string_t));
    }
}

myrtx_string_t* myrtx_string_create(myrtx_arena_t* arena, size_t initial_capacity) {
    /* Allocate the string structure */
    myrtx_string_t* str = string_struct_alloc(arena);
    if (!str) {
        return NULL;
    }
    
    /* Initialize the structure */
    str->data = NULL;
    str->length = 0;
    str->capacity = 0;
    str->arena = arena;
    
    /* Ensure minimum capacity (at least 1 byte for null terminator) */
    if (initial_capacity < 1) {
        initial_capacity = 16;  /* Default initial capacity */
    }
    
    /* Allocate memory for the string data */
    str->data = string_alloc(arena, initial_capacity);
    if (!str->data) {
        /* If using malloc, we need to free the structure */
        if (!arena) {
            free(str);
        }
        return NULL;
    }
    
    str->data[0] = '\0';
    str->capacity = initial_capacity;
    
    return str;
}

myrtx_string_t* myrtx_string_from_cstr(myrtx_arena_t* arena, const char* cstr) {
    if (!cstr) {
        /* Create an empty string */
        return myrtx_string_create(arena, 1);
    }
    
    size_t len = strlen(cstr);
    myrtx_string_t* str = myrtx_string_create(arena, len + 1);
    
    if (str) {
        memcpy(str->data, cstr, len + 1);
        str->length = len;
    }
    
    return str;
}

myrtx_string_t* myrtx_string_from_buffer(myrtx_arena_t* arena, const char* buffer, size_t length) {
    if (!buffer) {
        return myrtx_string_create(arena, 1);
    }
    
    myrtx_string_t* str = myrtx_string_create(arena, length + 1);
    
    if (str) {
        memcpy(str->data, buffer, length);
        str->data[length] = '\0';
        str->length = length;
    }
    
    return str;
}

myrtx_string_t* myrtx_string_format(myrtx_arena_t* arena, const char* format, ...) {
    if (!format) {
        return myrtx_string_create(arena, 1);
    }
    
    va_list args;
    va_start(args, format);
    
    /* First, determine the required size */
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (size < 0) {
        va_end(args);
        return NULL;
    }
    
    /* Create string with exact size needed */
    myrtx_string_t* str = myrtx_string_create(arena, size + 1);
    
    if (str) {
        vsnprintf(str->data, size + 1, format, args);
        str->length = size;
    }
    
    va_end(args);
    return str;
}

void myrtx_string_free(myrtx_string_t* str, bool force) {
    if (!str || !str->data) {
        return;
    }
    
    /* Free the string data if we're using malloc or force is true */
    if (!str->arena || force) {
        string_free(str->arena, str->data);
    }
    
    /* Reset the string struct */
    str->data = NULL;
    str->length = 0;
    str->capacity = 0;
    
    /* Free the structure itself if using malloc */
    if (!str->arena) {
        free(str);
    }
}

size_t myrtx_string_length(const myrtx_string_t* str) {
    return str ? str->length : 0;
}

size_t myrtx_string_capacity(const myrtx_string_t* str) {
    return str ? str->capacity : 0;
}

const char* myrtx_string_cstr(const myrtx_string_t* str) {
    if (!str || !str->data) {
        return "";  /* Return empty string for safety */
    }
    return str->data;
}

bool myrtx_string_set(myrtx_string_t* str, const char* cstr) {
    if (!str) {
        return false;
    }
    
    if (!cstr) {
        /* Set to empty string */
        if (str->data) {
            str->data[0] = '\0';
            str->length = 0;
        }
        return str->data != NULL;
    }
    
    size_t len = strlen(cstr);
    if (len + 1 > str->capacity) {
        if (!string_grow(str, len + 1)) {
            return false;
        }
    }
    
    memcpy(str->data, cstr, len + 1);
    str->length = len;
    
    return true;
}

bool myrtx_string_set_buffer(myrtx_string_t* str, const char* buffer, size_t length) {
    if (!str) {
        return false;
    }
    
    if (!buffer || length == 0) {
        /* Set to empty string */
        if (str->data) {
            str->data[0] = '\0';
            str->length = 0;
        }
        return str->data != NULL;
    }
    
    if (length + 1 > str->capacity) {
        if (!string_grow(str, length + 1)) {
            return false;
        }
    }
    
    memcpy(str->data, buffer, length);
    str->data[length] = '\0';  /* Ensure null termination */
    str->length = length;
    
    return true;
}

myrtx_string_t* myrtx_string_append(myrtx_string_t* str, const char* cstr) {
    if (!str || !cstr) {
        return NULL;
    }

    size_t cstr_len = strlen(cstr);
    if (cstr_len == 0) {
        return str;
    }

    if (!string_grow(str, str->length + cstr_len + 1)) {
        return NULL;
    }

    memcpy(str->data + str->length, cstr, cstr_len + 1);
    str->length += cstr_len;

    return str;
}

myrtx_string_t* myrtx_string_append_buffer(myrtx_string_t* str, const char* buffer, size_t length) {
    if (!str || !buffer) {
        return NULL;
    }

    if (length == 0) {
        return str;
    }

    if (!string_grow(str, str->length + length + 1)) {
        return NULL;
    }

    memcpy(str->data + str->length, buffer, length);
    str->length += length;
    str->data[str->length] = '\0';

    return str;
}

myrtx_string_t* myrtx_string_append_char(myrtx_string_t* str, char c) {
    if (!str) {
        return NULL;
    }

    if (!string_grow(str, str->length + 2)) {
        return NULL;
    }

    str->data[str->length++] = c;
    str->data[str->length] = '\0';

    return str;
}

bool myrtx_string_append_format(myrtx_string_t* str, const char* format, ...) {
    if (!str || !format) {
        return false;
    }
    
    va_list args;
    va_start(args, format);
    
    /* First pass: determine the required buffer size */
    va_list args_copy;
    va_copy(args_copy, args);
    int format_size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (format_size < 0) {
        va_end(args);
        return false; /* Format error */
    }
    
    size_t new_length = str->length + (size_t)format_size;
    if (new_length + 1 > str->capacity) {
        if (!string_grow(str, new_length + 1)) {
            va_end(args);
            return false;
        }
    }
    
    /* Format the string directly into the buffer at the current position */
    vsnprintf(str->data + str->length, (size_t)format_size + 1, format, args);
    va_end(args);
    
    str->length = new_length;
    
    return true;
}

int myrtx_string_compare(const myrtx_string_t* str1, const myrtx_string_t* str2) {
    if (!str1 || !str1->data) {
        return (str2 && str2->data) ? -1 : 0;
    }
    
    if (!str2 || !str2->data) {
        return 1;
    }
    
    return strcmp(str1->data, str2->data);
}

bool myrtx_string_is_empty(const myrtx_string_t* str) {
    return !str || !str->data || str->length == 0;
}

void myrtx_string_clear(myrtx_string_t* str) {
    if (str && str->data) {
        str->data[0] = '\0';
        str->length = 0;
    }
}

myrtx_string_t* myrtx_string_substr(myrtx_arena_t* arena, const myrtx_string_t* str, size_t start, size_t length) {
    if (!str || start >= str->length) {
        return NULL;
    }

    // If length is 0 or exceeds string length, use remaining length
    if (length == 0 || start + length > str->length) {
        length = str->length - start;
    }

    myrtx_string_t* result = string_struct_alloc(arena);
    if (!result) {
        return NULL;
    }

    result->data = string_alloc(arena, length + 1);
    if (!result->data) {
        string_free(arena, result);
        return NULL;
    }

    memcpy(result->data, str->data + start, length);
    result->data[length] = '\0';
    result->length = length;
    result->capacity = length + 1;
    result->arena = arena;

    return result;
}

myrtx_string_t* myrtx_string_clone(myrtx_arena_t* arena, const myrtx_string_t* str) {
    if (!str) {
        return NULL;
    }

    myrtx_string_t* result = string_struct_alloc(arena);
    if (!result) {
        return NULL;
    }

    result->data = string_alloc(arena, str->capacity);
    if (!result->data) {
        string_free(arena, result);
        return NULL;
    }

    memcpy(result->data, str->data, str->length + 1);
    result->length = str->length;
    result->capacity = str->capacity;
    result->arena = arena;

    return result;
}

bool myrtx_string_reserve(myrtx_string_t* str, size_t new_capacity) {
    if (!str) {
        return false;
    }
    
    /* Ensure we don't shrink below the current length + null terminator */
    if (new_capacity < str->length + 1) {
        new_capacity = str->length + 1;
    }
    
    /* If new capacity is smaller than current, do nothing */
    if (new_capacity <= str->capacity) {
        return true;
    }
    
    return string_grow(str, new_capacity);
}

bool myrtx_string_trim(myrtx_string_t* str) {
    if (!str || !str->data || str->length == 0) {
        return true;  /* Nothing to trim */
    }
    
    /* Find the first non-whitespace character */
    size_t start = 0;
    while (start < str->length && isspace((unsigned char)str->data[start])) {
        start++;
    }
    
    /* If we reached the end, the string is all whitespace */
    if (start == str->length) {
        str->data[0] = '\0';
        str->length = 0;
        return true;
    }
    
    /* Find the last non-whitespace character */
    size_t end = str->length - 1;
    while (end > start && isspace((unsigned char)str->data[end])) {
        end--;
    }
    
    /* Calculate length of trimmed string */
    size_t new_length = end - start + 1;
    
    /* Move the content if we're not starting from the beginning */
    if (start > 0) {
        memmove(str->data, str->data + start, new_length);
    }
    
    str->data[new_length] = '\0';
    str->length = new_length;
    
    return true;
}

bool myrtx_string_to_upper(myrtx_string_t* str) {
    if (!str || !str->data) {
        return false;
    }
    
    for (size_t i = 0; i < str->length; i++) {
        str->data[i] = (char)toupper((unsigned char)str->data[i]);
    }
    
    return true;
}

bool myrtx_string_to_lower(myrtx_string_t* str) {
    if (!str || !str->data) {
        return false;
    }
    
    for (size_t i = 0; i < str->length; i++) {
        str->data[i] = (char)tolower((unsigned char)str->data[i]);
    }
    
    return true;
}

bool myrtx_string_starts_with(const myrtx_string_t* str, const char* prefix) {
    if (!str || !str->data || !prefix) {
        return false;
    }
    
    size_t prefix_len = strlen(prefix);
    if (prefix_len > str->length) {
        return false;
    }
    
    return memcmp(str->data, prefix, prefix_len) == 0;
}

bool myrtx_string_ends_with(const myrtx_string_t* str, const char* suffix) {
    if (!str || !str->data || !suffix) {
        return false;
    }
    
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str->length) {
        return false;
    }
    
    return memcmp(str->data + (str->length - suffix_len), suffix, suffix_len) == 0;
}

size_t myrtx_string_find(const myrtx_string_t* str, const char* substr) {
    return myrtx_string_find_from(str, substr, 0);
}

size_t myrtx_string_find_from(const myrtx_string_t* str, const char* substr, size_t pos) {
    if (!str || !str->data || !substr || pos >= str->length) {
        return SIZE_MAX;
    }
    
    const char* found = strstr(str->data + pos, substr);
    if (!found) {
        return SIZE_MAX;
    }
    
    return (size_t)(found - str->data);
}

size_t myrtx_string_rfind(const myrtx_string_t* str, const char* substr) {
    if (!str || !str->data || !substr) {
        return SIZE_MAX;
    }
    
    size_t substr_len = strlen(substr);
    if (substr_len == 0 || substr_len > str->length) {
        return SIZE_MAX;
    }
    
    /* Start searching from the end of the string */
    for (size_t i = str->length - substr_len + 1; i > 0; i--) {
        size_t pos = i - 1;
        if (memcmp(str->data + pos, substr, substr_len) == 0) {
            return pos;
        }
    }
    
    return SIZE_MAX;
}

bool myrtx_string_replace(myrtx_string_t* str, const char* old_str, const char* new_str) {
    if (!str || !str->data || !old_str || !new_str) {
        return false;
    }
    
    size_t old_len = strlen(old_str);
    if (old_len == 0) {
        return true;  /* Nothing to replace */
    }
    
    /* Ermittle die neue String-Länge */
    size_t new_len = strlen(new_str);
    
    /* Find first occurrence */
    size_t pos = myrtx_string_find(str, old_str);
    if (pos == SIZE_MAX) {
        return true;  /* No occurrences found */
    }
    
    /* Create a temporary string for building the result */
    myrtx_string_t* temp = myrtx_string_create(str->arena, str->length);
    if (!temp) {
        return false;
    }
    
    size_t last_pos = 0;
    
    /* Iterate through all occurrences */
    while (pos != SIZE_MAX) {
        /* Append text up to the occurrence */
        myrtx_string_append_buffer(temp, str->data + last_pos, pos - last_pos);
        
        /* Append replacement text using new_len */
        if (new_len > 0) {
            myrtx_string_append_buffer(temp, new_str, new_len);
        }
        
        /* Update position for next search */
        last_pos = pos + old_len;
        
        /* Find next occurrence */
        pos = myrtx_string_find_from(str, old_str, last_pos);
    }
    
    /* Append trailing text */
    if (last_pos < str->length) {
        myrtx_string_append_buffer(temp, str->data + last_pos, str->length - last_pos);
    }
    
    /* Use the result to replace the content of the original string */
    if (str->arena) {
        /* With an arena, we allocate new memory */
        char* new_data = (char*)myrtx_arena_alloc(str->arena, temp->length + 1);
        if (!new_data) {
            myrtx_string_free(temp, true);
            return false;
        }
        
        memcpy(new_data, temp->data, temp->length + 1);
        str->data = new_data;
    } else {
        /* With malloc, we can swap the buffers */
        char* temp_data = str->data;
        str->data = temp->data;
        temp->data = temp_data;
    }
    
    str->length = temp->length;
    str->capacity = temp->capacity;
    
    /* Free the temporary string */
    myrtx_string_free(temp, true);
    
    return true;
}

myrtx_string_t* myrtx_string_split(myrtx_arena_t* arena, const myrtx_string_t* str, const char* delimiter, size_t* count) {
    if (!arena || !str || !str->data || !delimiter || !count) {
        return NULL;
    }
    
    size_t delimiter_len = strlen(delimiter);
    if (delimiter_len == 0) {
        /* Empty delimiter: split into individual characters */
        *count = str->length;
        myrtx_string_t* result = (myrtx_string_t*)myrtx_arena_alloc(arena, (*count) * sizeof(myrtx_string_t));
        if (!result) {
            *count = 0;
            return NULL;
        }
        
        for (size_t i = 0; i < str->length; i++) {
            myrtx_string_t* temp = myrtx_string_from_buffer(arena, &str->data[i], 1);
            if (!temp) {
                *count = 0;
                return NULL;
            }
            result[i] = *temp;
            myrtx_string_free(temp, true);
        }
        
        return result;
    }
    
    /* First, count the number of delimiters to determine the number of parts */
    size_t parts = 1;  /* Start with 1 part (even empty string is one part) */
    size_t pos = 0;
    
    while ((pos = myrtx_string_find_from(str, delimiter, pos)) != SIZE_MAX) {
        parts++;
        pos += delimiter_len;
    }
    
    /* Allocate array for the parts */
    myrtx_string_t* result = (myrtx_string_t*)myrtx_arena_alloc(arena, parts * sizeof(myrtx_string_t));
    if (!result) {
        *count = 0;
        return NULL;
    }
    
    /* Split the string */
    size_t start = 0;
    size_t part = 0;
    pos = 0;
    
    while ((pos = myrtx_string_find_from(str, delimiter, start)) != SIZE_MAX) {
        /* Extract part */
        size_t part_len = pos - start;
        
        /* Handle empty part specially */
        if (part_len == 0) {
            /* Create empty string directly */
            result[part].data = (char*)myrtx_arena_alloc(arena, 1);
            if (!result[part].data) {
                *count = 0;
                return NULL;
            }
            result[part].data[0] = '\0';
            result[part].length = 0;
            result[part].capacity = 1;
            result[part].arena = arena;
        } else {
            /* Extract non-empty part */
            myrtx_string_t* temp = myrtx_string_substr(arena, str, start, part_len);
            if (!temp) {
                *count = 0;
                return NULL;
            }
            result[part] = *temp;
            myrtx_string_free(temp, true);
        }
        
        part++;
        start = pos + delimiter_len;
    }
    
    /* Extract the last part */
    size_t last_part_len = str->length - start;
    if (last_part_len == 0) {
        /* Create empty string directly for last part */
        result[part].data = (char*)myrtx_arena_alloc(arena, 1);
        if (!result[part].data) {
            *count = 0;
            return NULL;
        }
        result[part].data[0] = '\0';
        result[part].length = 0;
        result[part].capacity = 1;
        result[part].arena = arena;
    } else {
        /* Extract non-empty last part */
        myrtx_string_t* temp = myrtx_string_substr(arena, str, start, last_part_len);
        if (!temp) {
            *count = 0;
            return NULL;
        }
        result[part] = *temp;
        myrtx_string_free(temp, true);
    }
    
    *count = parts;
    return result;
}

myrtx_string_t* myrtx_string_join(myrtx_arena_t* arena, const myrtx_string_t* strings, size_t count, const char* delimiter) {
    if (!strings || count == 0) {
        return NULL;
    }

    // Calculate total length needed
    size_t total_length = 0;
    size_t delimiter_len = delimiter ? strlen(delimiter) : 0;
    
    for (size_t i = 0; i < count; i++) {
        if (strings[i].data) {
            total_length += strings[i].length;
            if (i < count - 1 && delimiter_len > 0) {
                total_length += delimiter_len;
            }
        }
    }

    myrtx_string_t* result = string_struct_alloc(arena);
    if (!result) {
        return NULL;
    }

    result->data = string_alloc(arena, total_length + 1);
    if (!result->data) {
        string_free(arena, result);
        return NULL;
    }

    char* current = result->data;
    for (size_t i = 0; i < count; i++) {
        if (strings[i].data) {
            memcpy(current, strings[i].data, strings[i].length);
            current += strings[i].length;
            
            if (i < count - 1 && delimiter_len > 0) {
                memcpy(current, delimiter, delimiter_len);
                current += delimiter_len;
            }
        }
    }

    *current = '\0';
    result->length = total_length;
    result->capacity = total_length + 1;
    result->arena = arena;

    return result;
}

/* Legacy C string functions - Original implementation remains unchanged */

char* myrtx_strdup(myrtx_arena_t* arena, const char* str) {
    if (!arena || !str) {
        return NULL;
    }
    
    size_t len = strlen(str) + 1; /* +1 for null terminator */
    char* result = (char*)myrtx_arena_alloc(arena, len);
    if (!result) {
        return NULL;
    }
    
    memcpy(result, str, len);
    return result;
}

char* myrtx_strndup(myrtx_arena_t* arena, const char* str, size_t n) {
    if (!arena || !str) {
        return NULL;
    }
    
    size_t len = strlen(str);
    if (len > n) {
        len = n;
    }
    
    char* result = (char*)myrtx_arena_alloc(arena, len + 1); /* +1 for null terminator */
    if (!result) {
        return NULL;
    }
    
    memcpy(result, str, len);
    result[len] = '\0'; /* Ensure null termination */
    
    return result;
}

char* myrtx_strfmt(myrtx_arena_t* arena, const char* format, ...) {
    if (!arena || !format) {
        return NULL;
    }
    
    /* First pass: determine the required buffer size */
    va_list args;
    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    
    if (size < 0) {
        return NULL; /* Format error */
    }
    
    /* Allocate buffer (add 1 for null terminator) */
    char* result = (char*)myrtx_arena_alloc(arena, (size_t)size + 1);
    if (!result) {
        return NULL;
    }
    
    /* Second pass: format the string into the buffer */
    va_start(args, format);
    vsnprintf(result, (size_t)size + 1, format, args);
    va_end(args);
    
    return result;
}

char* myrtx_strcat_dup(myrtx_arena_t* arena, const char* str1, const char* str2) {
    if (!arena || !str1 || !str2) {
        return NULL;
    }
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t total_len = len1 + len2 + 1; /* +1 for null terminator */
    
    char* result = (char*)myrtx_arena_alloc(arena, total_len);
    if (!result) {
        return NULL;
    }
    
    memcpy(result, str1, len1);
    memcpy(result + len1, str2, len2 + 1); /* +1 to include null terminator */
    
    return result;
}

char** myrtx_strsplit(myrtx_arena_t* arena, const char* str, const char* delimiters, size_t* count) {
    if (!arena || !str || !delimiters || !count) {
        return NULL;
    }
    
    /* First count how many tokens we'll have */
    size_t token_count = 1; /* Start with 1 token (there's at least one token if string is not empty) */
    const char* p;
    
    /* Count delimiters to determine number of tokens */
    for (p = str; *p; p++) {
        if (strchr(delimiters, *p) != NULL) {
            token_count++;
        }
    }
    
    /* Handle empty string case */
    if (*str == '\0') {
        token_count = 0;
    }
    
    /* Allocate the result array (plus 1 for NULL terminator) */
    char** result = (char**)myrtx_arena_alloc(arena, (token_count + 1) * sizeof(char*));
    if (!result) {
        return NULL;
    }
    
    /* Set the last pointer to NULL for easier iteration */
    result[token_count] = NULL;
    
    /* Handle empty string case */
    if (token_count == 0) {
        *count = 0;
        return result;
    }
    
    /* Actually split the string */
    p = str;
    size_t i = 0;
    
    const char* token_start = p;
    
    for (;;) {
        if (*p == '\0' || strchr(delimiters, *p) != NULL) {
            /* Found a delimiter or end of string, extract the token */
            size_t token_length = p - token_start;
            
            /* Allocate memory for the token */
            result[i] = (char*)myrtx_arena_alloc(arena, token_length + 1);
            if (!result[i]) {
                *count = 0;
                return NULL;
            }
            
            /* Copy token and null-terminate it */
            if (token_length > 0) {
                memcpy(result[i], token_start, token_length);
            }
            result[i][token_length] = '\0';
            i++;
            
            /* Exit if we reached the end of the string */
            if (*p == '\0') {
                break;
            }
            
            /* Move to next character after delimiter for next token */
            token_start = p + 1;
        }
        
        p++;
    }
    
    /* In case we had trailing delimiters, adjust the token count */
    *count = i;
    return result;
}

char* myrtx_strjoin(myrtx_arena_t* arena, const char** strings, size_t count, const char* separator) {
    if (!arena || !strings || !separator) {
        return NULL;
    }
    
    if (count == 0) {
        /* Return an empty string */
        char* result = (char*)myrtx_arena_alloc(arena, 1);
        if (result) {
            result[0] = '\0';
        }
        return result;
    }
    
    /* Calculate total length */
    size_t sep_len = strlen(separator);
    size_t total_len = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (!strings[i]) {
            return NULL;
        }
        
        total_len += strlen(strings[i]);
        if (i < count - 1) {
            total_len += sep_len;
        }
    }
    
    /* Allocate result buffer with space for null terminator */
    char* result = (char*)myrtx_arena_alloc(arena, total_len + 1);
    if (!result) {
        return NULL;
    }
    
    /* Join strings */
    char* dst = result;
    for (size_t i = 0; i < count; i++) {
        size_t len = strlen(strings[i]);
        memcpy(dst, strings[i], len);
        dst += len;
        
        if (i < count - 1) {
            memcpy(dst, separator, sep_len);
            dst += sep_len;
        }
    }
    
    /* Null-terminate the result */
    *dst = '\0';
    
    return result;
}

char* myrtx_substr(myrtx_arena_t* arena, const char* str, size_t start, size_t length) {
    if (!arena || !str) {
        return NULL;
    }
    
    size_t str_len = strlen(str);
    
    /* Check if start is beyond the end of the string */
    if (start >= str_len) {
        char* result = (char*)myrtx_arena_alloc(arena, 1);
        if (result) {
            result[0] = '\0';
        }
        return result;
    }
    
    /* Adjust length if it would go beyond the end of the string */
    if (start + length > str_len) {
        length = str_len - start;
    }
    
    /* Allocate space for the substring plus null terminator */
    char* result = (char*)myrtx_arena_alloc(arena, length + 1);
    if (!result) {
        return NULL;
    }
    
    /* Copy substring */
    memcpy(result, str + start, length);
    result[length] = '\0';
    
    return result;
}

char* myrtx_strtoupper(myrtx_arena_t* arena, const char* str) {
    if (!arena || !str) {
        return NULL;
    }
    
    size_t len = strlen(str);
    char* result = (char*)myrtx_arena_alloc(arena, len + 1);
    if (!result) {
        return NULL;
    }
    
    /* Convert each character to uppercase */
    for (size_t i = 0; i < len; i++) {
        result[i] = (char)toupper((unsigned char)str[i]);
    }
    
    result[len] = '\0';
    return result;
}

char* myrtx_strtolower(myrtx_arena_t* arena, const char* str) {
    if (!arena || !str) {
        return NULL;
    }
    
    size_t len = strlen(str);
    char* result = (char*)myrtx_arena_alloc(arena, len + 1);
    if (!result) {
        return NULL;
    }
    
    /* Convert each character to lowercase */
    for (size_t i = 0; i < len; i++) {
        result[i] = (char)tolower((unsigned char)str[i]);
    }
    
    result[len] = '\0';
    return result;
}

bool myrtx_startswith(const char* str, const char* prefix) {
    if (!str || !prefix) {
        return false;
    }
    
    size_t prefix_len = strlen(prefix);
    if (strncmp(str, prefix, prefix_len) == 0) {
        return true;
    }
    
    return false;
}

bool myrtx_endswith(const char* str, const char* suffix) {
    if (!str || !suffix) {
        return false;
    }
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return false;
    }
    
    return (strcmp(str + (str_len - suffix_len), suffix) == 0);
}

char* myrtx_strtrim(myrtx_arena_t* arena, const char* str) {
    if (!arena || !str) {
        return NULL;
    }
    
    /* Find the first non-whitespace character */
    const char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    
    /* If we reached the end, the string is all whitespace */
    if (*start == '\0') {
        char* result = (char*)myrtx_arena_alloc(arena, 1);
        if (result) {
            result[0] = '\0';
        }
        return result;
    }
    
    /* Find the last non-whitespace character */
    const char* end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    
    /* Calculate length of trimmed string */
    size_t length = end - start + 1;
    
    /* Allocate and copy the trimmed string */
    char* result = (char*)myrtx_arena_alloc(arena, length + 1);
    if (!result) {
        return NULL;
    }
    
    memcpy(result, start, length);
    result[length] = '\0';
    
    return result;
}

char* myrtx_strreplace(myrtx_arena_t* arena, const char* str, const char* old_str, const char* new_str) {
    if (!str || !old_str || !new_str) {
        return NULL;
    }
    
    size_t old_len = strlen(old_str);
    
    /* If old string is empty, just duplicate the input string */
    if (old_len == 0) {
        return myrtx_strdup(arena, str);
    }
    
    /* Calculate total size */
    size_t len = strlen(str);
    const char* p = str;
    size_t count = 0;
    
    /* Count occurrences of old_str */
    while ((p = strstr(p, old_str)) != NULL) {
        count++;
        p += old_len;
    }
    
    /* If no occurrences, just duplicate the input string */
    if (count == 0) {
        return myrtx_strdup(arena, str);
    }
    
    /* Calculate final size for new string */
    /* We remove old_len * count bytes and add new_len * count bytes */
    size_t new_len = strlen(new_str);
    size_t final_size = len + (new_len - old_len) * count + 1;
    
    /* Allocate memory for the result */
    char* result = myrtx_arena_alloc(arena, final_size);
    if (!result) {
        return NULL;
    }
    
    /* Perform replacements */
    char* dst = result;
    const char* src = str;
    const char* match;
    
    while ((match = strstr(src, old_str)) != NULL) {
        /* Copy everything up to the match */
        size_t copy_len = match - src;
        memcpy(dst, src, copy_len);
        dst += copy_len;
        
        /* Copy the replacement */
        memcpy(dst, new_str, new_len);
        dst += new_len;
        
        /* Move past the match in the source */
        src = match + old_len;
    }
    
    /* Copy the remaining part of the input string */
    strcpy(dst, src);
    
    return result;
} 