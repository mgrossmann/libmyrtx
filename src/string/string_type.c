#include "myrtx/string/string.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

/*
 * String type implementation
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

myrtx_string_t myrtx_string_create(myrtx_arena_t* arena, size_t initial_capacity) {
    myrtx_string_t str = {NULL, 0, 0, arena};
    
    /* Ensure minimum capacity (at least 1 byte for null terminator) */
    if (initial_capacity < 1) {
        initial_capacity = 16;  /* Default initial capacity */
    }
    
    /* Allocate memory for the string */
    str.data = string_alloc(arena, initial_capacity);
    if (str.data) {
        str.data[0] = '\0';
        str.capacity = initial_capacity;
    }
    
    return str;
}

myrtx_string_t myrtx_string_from_cstr(myrtx_arena_t* arena, const char* cstr) {
    myrtx_string_t str = {NULL, 0, 0, arena};
    
    if (!cstr) {
        /* Create an empty string */
        return myrtx_string_create(arena, 1);
    }
    
    size_t len = strlen(cstr);
    str.data = string_alloc(arena, len + 1);
    
    if (str.data) {
        memcpy(str.data, cstr, len + 1);
        str.length = len;
        str.capacity = len + 1;
    }
    
    return str;
}

myrtx_string_t myrtx_string_from_buffer(myrtx_arena_t* arena, const char* buffer, size_t length) {
    myrtx_string_t str = {NULL, 0, 0, arena};
    
    if (!buffer || length == 0) {
        /* Create an empty string */
        return myrtx_string_create(arena, 1);
    }
    
    str.data = string_alloc(arena, length + 1);
    
    if (str.data) {
        memcpy(str.data, buffer, length);
        str.data[length] = '\0';  /* Ensure null termination */
        str.length = length;
        str.capacity = length + 1;
    }
    
    return str;
}

myrtx_string_t myrtx_string_format(myrtx_arena_t* arena, const char* format, ...) {
    myrtx_string_t str = {NULL, 0, 0, arena};
    
    if (!format) {
        return str;
    }
    
    va_list args;
    va_start(args, format);
    
    /* First pass: determine the required buffer size */
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (size < 0) {
        va_end(args);
        return str; /* Format error */
    }
    
    /* Allocate buffer (add 1 for null terminator) */
    str.data = string_alloc(arena, (size_t)size + 1);
    if (!str.data) {
        va_end(args);
        return str;
    }
    
    /* Second pass: format the string into the buffer */
    vsnprintf(str.data, (size_t)size + 1, format, args);
    va_end(args);
    
    str.length = (size_t)size;
    str.capacity = (size_t)size + 1;
    
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

bool myrtx_string_append(myrtx_string_t* str, const char* cstr) {
    if (!str || !cstr) {
        return false;
    }
    
    size_t len = strlen(cstr);
    if (len == 0) {
        return true;  /* Nothing to append */
    }
    
    size_t new_length = str->length + len;
    if (new_length + 1 > str->capacity) {
        if (!string_grow(str, new_length + 1)) {
            return false;
        }
    }
    
    memcpy(str->data + str->length, cstr, len + 1);
    str->length = new_length;
    
    return true;
}

bool myrtx_string_append_buffer(myrtx_string_t* str, const char* buffer, size_t length) {
    if (!str || !buffer || length == 0) {
        return false;
    }
    
    size_t new_length = str->length + length;
    if (new_length + 1 > str->capacity) {
        if (!string_grow(str, new_length + 1)) {
            return false;
        }
    }
    
    memcpy(str->data + str->length, buffer, length);
    str->data[new_length] = '\0';  /* Ensure null termination */
    str->length = new_length;
    
    return true;
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

myrtx_string_t myrtx_string_substr(myrtx_arena_t* arena, const myrtx_string_t* str, size_t start, size_t length) {
    myrtx_string_t result = {NULL, 0, 0, arena};
    
    if (!str || !str->data) {
        return result;
    }
    
    /* Check if start is beyond the end of the string */
    if (start >= str->length) {
        /* Return an empty string */
        return myrtx_string_create(arena, 1);
    }
    
    /* Adjust length if it would go beyond the end of the string */
    if (start + length > str->length) {
        length = str->length - start;
    }
    
    /* Allocate memory for the substring */
    result.data = string_alloc(arena, length + 1);
    if (!result.data) {
        return result;
    }
    
    /* Copy substring */
    memcpy(result.data, str->data + start, length);
    result.data[length] = '\0';
    result.length = length;
    result.capacity = length + 1;
    
    return result;
}

myrtx_string_t myrtx_string_clone(myrtx_arena_t* arena, const myrtx_string_t* str) {
    if (!str || !str->data) {
        return myrtx_string_create(arena, 1);
    }
    
    return myrtx_string_from_buffer(arena, str->data, str->length);
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

bool myrtx_string_replace(myrtx_string_t* str, const char* old_str, const char* new_str) {
    if (!str || !str->data || !old_str || !new_str) {
        return false;
    }
    
    size_t old_len = strlen(old_str);
    if (old_len == 0) {
        return true;  /* Nothing to replace */
    }
    
    /* Create a buffer for the result */
    myrtx_string_t result = myrtx_string_create(str->arena, str->length);
    if (!result.data) {
        return false;
    }
    
    const char* pos = str->data;
    const char* find;
    
    /* Find all occurrences and build the result */
    while ((find = strstr(pos, old_str)) != NULL) {
        /* Append text up to the match */
        size_t prefix_len = find - pos;
        if (prefix_len > 0) {
            if (!myrtx_string_append_buffer(&result, pos, prefix_len)) {
                myrtx_string_free(&result, true);
                return false;
            }
        }
        
        /* Append replacement text */
        if (!myrtx_string_append(&result, new_str)) {
            myrtx_string_free(&result, true);
            return false;
        }
        
        /* Advance position past the match */
        pos = find + old_len;
    }
    
    /* Append trailing text */
    if (*pos != '\0') {
        if (!myrtx_string_append(&result, pos)) {
            myrtx_string_free(&result, true);
            return false;
        }
    }
    
    /* Replace original string with result */
    if (str->arena) {
        /* With arena, we need to allocate new memory */
        char* new_data = string_alloc(str->arena, result.length + 1);
        if (!new_data) {
            myrtx_string_free(&result, true);
            return false;
        }
        
        memcpy(new_data, result.data, result.length + 1);
        str->data = new_data;
    } else {
        /* With malloc, just swap the buffers */
        char* tmp = str->data;
        str->data = result.data;
        result.data = tmp;
        
        size_t tmp_capacity = str->capacity;
        str->capacity = result.capacity;
        result.capacity = tmp_capacity;
    }
    
    str->length = result.length;
    myrtx_string_free(&result, true);
    
    return true;
}

size_t myrtx_string_find(const myrtx_string_t* str, const char* substr) {
    if (!str || !str->data || !substr) {
        return SIZE_MAX;
    }
    
    const char* found = strstr(str->data, substr);
    if (!found) {
        return SIZE_MAX;
    }
    
    return (size_t)(found - str->data);
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

myrtx_string_t* myrtx_string_split(myrtx_arena_t* arena, const myrtx_string_t* str, const char* delimiter, size_t* count) {
    if (!arena || !str || !str->data || !delimiter || !count) {
        return NULL;
    }
    
    size_t delimiter_len = strlen(delimiter);
    if (delimiter_len == 0 || str->length == 0) {
        /* Special cases */
        myrtx_string_t* result;
        
        if (str->length == 0) {
            /* Empty string -> return empty array */
            *count = 0;
            return NULL;
        } else if (delimiter_len == 0) {
            /* Empty delimiter -> split into individual characters */
            *count = str->length;
            result = (myrtx_string_t*)myrtx_arena_alloc(arena, (*count) * sizeof(myrtx_string_t));
            if (!result) {
                *count = 0;
                return NULL;
            }
            
            for (size_t i = 0; i < str->length; i++) {
                result[i] = myrtx_string_from_buffer(arena, &str->data[i], 1);
                if (!result[i].data) {
                    *count = 0;
                    return NULL;
                }
            }
            
            return result;
        }
    }
    
    /* Count the number of parts */
    size_t part_count = 1;  /* Start with 1 for the first part */
    const char* p = str->data;
    const char* found;
    
    while ((found = strstr(p, delimiter)) != NULL) {
        part_count++;
        p = found + delimiter_len;
    }
    
    /* Allocate array for results */
    myrtx_string_t* result = (myrtx_string_t*)myrtx_arena_alloc(arena, part_count * sizeof(myrtx_string_t));
    if (!result) {
        *count = 0;
        return NULL;
    }
    
    /* Split the string */
    p = str->data;
    for (size_t i = 0; i < part_count; i++) {
        found = strstr(p, delimiter);
        
        if (found) {
            /* Not the last part */
            size_t part_len = found - p;
            result[i] = myrtx_string_from_buffer(arena, p, part_len);
            if (!result[i].data) {
                *count = 0;
                return NULL;
            }
            
            p = found + delimiter_len;
        } else {
            /* Last part */
            result[i] = myrtx_string_from_cstr(arena, p);
            if (!result[i].data) {
                *count = 0;
                return NULL;
            }
            
            break;
        }
    }
    
    *count = part_count;
    return result;
}

myrtx_string_t myrtx_string_join(myrtx_arena_t* arena, const myrtx_string_t* strings, size_t count, const char* delimiter) {
    if (!strings || count == 0) {
        /* Return an empty string */
        return myrtx_string_create(arena, 1);
    }
    
    if (!delimiter) {
        delimiter = "";
    }
    
    size_t delimiter_len = strlen(delimiter);
    
    /* Calculate the total length of the joined string */
    size_t total_len = 0;
    for (size_t i = 0; i < count; i++) {
        if (strings[i].data) {
            total_len += strings[i].length;
            if (i < count - 1) {
                total_len += delimiter_len;
            }
        }
    }
    
    /* Create the result string */
    myrtx_string_t result = myrtx_string_create(arena, total_len + 1);
    if (!result.data) {
        return result;
    }
    
    /* Join the strings */
    for (size_t i = 0; i < count; i++) {
        if (strings[i].data) {
            myrtx_string_append_buffer(&result, strings[i].data, strings[i].length);
            if (i < count - 1 && delimiter_len > 0) {
                myrtx_string_append(&result, delimiter);
            }
        }
    }
    
    return result;
} 