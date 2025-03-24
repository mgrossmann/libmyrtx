#include "myrtx/string/string.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

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
    if (!arena || !str || !old_str || !new_str) {
        return NULL;
    }
    
    size_t old_len = strlen(old_str);
    if (old_len == 0) {
        /* Cannot replace an empty string */
        return myrtx_strdup(arena, str);
    }
    
    size_t new_len = strlen(new_str);
    
    /* Count occurrences of old_str in str */
    size_t count = 0;
    const char* p = str;
    
    while ((p = strstr(p, old_str)) != NULL) {
        count++;
        p += old_len;
    }
    
    if (count == 0) {
        /* No replacements needed */
        return myrtx_strdup(arena, str);
    }
    
    /* Calculate the new string length */
    size_t str_len = strlen(str);
    size_t result_len = str_len + (new_len - old_len) * count;
    
    /* Allocate memory for the result */
    char* result = (char*)myrtx_arena_alloc(arena, result_len + 1);
    if (!result) {
        return NULL;
    }
    
    /* Perform replacement */
    char* dst = result;
    const char* src = str;
    const char* match;
    
    while ((match = strstr(src, old_str)) != NULL) {
        /* Copy everything up to the match */
        size_t len = match - src;
        memcpy(dst, src, len);
        dst += len;
        
        /* Copy the replacement */
        memcpy(dst, new_str, new_len);
        dst += new_len;
        
        /* Move past the match in the source */
        src = match + old_len;
    }
    
    /* Copy the remainder of the source string */
    strcpy(dst, src);
    
    return result;
} 