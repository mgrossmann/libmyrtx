/**
 * @file string_example.c
 * @brief Example usage of myrtx string object type
 */

#include "myrtx/string/string.h"
#include <stdio.h>

int main(void) {
    printf("=== String Example ===\n\n");
    
    /* Initialize arena */
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Failed to initialize arena\n");
        return 1;
    }
    
    /* Create strings in different ways */
    printf("Creating strings:\n");
    
    /* Create an empty string */
    myrtx_string_t* empty = myrtx_string_create(&arena, 10);
    printf("Empty string: \"%s\", length: %zu, capacity: %zu\n", 
           myrtx_string_cstr(empty), 
           myrtx_string_length(empty),
           myrtx_string_capacity(empty));
    
    /* Create from C string */
    myrtx_string_t* from_cstr = myrtx_string_from_cstr(&arena, "Hello, world!");
    printf("From C string: \"%s\", length: %zu, capacity: %zu\n", 
           myrtx_string_cstr(from_cstr), 
           myrtx_string_length(from_cstr),
           myrtx_string_capacity(from_cstr));
    
    /* Create from buffer with explicit length */
    const char buffer[] = {'H', 'e', 'l', 'l', 'o', '\0', 'W', 'o', 'r', 'l', 'd'};
    myrtx_string_t* from_buffer = myrtx_string_from_buffer(&arena, buffer, sizeof(buffer));
    printf("From buffer: \"%s\", length: %zu, capacity: %zu\n", 
           myrtx_string_cstr(from_buffer), 
           myrtx_string_length(from_buffer),
           myrtx_string_capacity(from_buffer));
    
    /* Create with formatting */
    myrtx_string_t* formatted = myrtx_string_format(&arena, "The answer is %d", 42);
    printf("Formatted: \"%s\", length: %zu, capacity: %zu\n\n", 
           myrtx_string_cstr(formatted), 
           myrtx_string_length(formatted),
           myrtx_string_capacity(formatted));
    
    /* Modifying strings */
    printf("Modifying strings:\n");
    
    /* Set content from C string */
    myrtx_string_t* modifiable = myrtx_string_create(&arena, 32);
    myrtx_string_set(modifiable, "Initial content");
    printf("Initial: \"%s\", length: %zu, capacity: %zu\n", 
           myrtx_string_cstr(modifiable), 
           myrtx_string_length(modifiable),
           myrtx_string_capacity(modifiable));
    
    /* Append text */
    myrtx_string_append(modifiable, " - Appended text");
    printf("After append: \"%s\", length: %zu, capacity: %zu\n", 
           myrtx_string_cstr(modifiable), 
           myrtx_string_length(modifiable),
           myrtx_string_capacity(modifiable));
    
    /* Append formatted text */
    myrtx_string_append_format(modifiable, " [%d]", 123);
    printf("After append format: \"%s\", length: %zu, capacity: %zu\n\n", 
           myrtx_string_cstr(modifiable), 
           myrtx_string_length(modifiable),
           myrtx_string_capacity(modifiable));
    
    /* String transformations */
    printf("String transformations:\n");
    
    /* Clone a string */
    myrtx_string_t* clone = myrtx_string_clone(&arena, from_cstr);
    printf("Original: \"%s\"\n", myrtx_string_cstr(from_cstr));
    printf("Clone: \"%s\"\n", myrtx_string_cstr(clone));
    
    /* Convert to uppercase */
    myrtx_string_to_upper(clone);
    printf("Uppercase: \"%s\"\n", myrtx_string_cstr(clone));
    
    /* Convert to lowercase */
    myrtx_string_to_lower(clone);
    printf("Lowercase: \"%s\"\n", myrtx_string_cstr(clone));
    
    /* Create a substring */
    myrtx_string_t* substr = myrtx_string_substr(&arena, from_cstr, 7, 5);
    printf("Substring (7,5): \"%s\"\n\n", myrtx_string_cstr(substr));
    
    /* String operations */
    printf("String operations:\n");
    
    /* Check if string starts/ends with */
    printf("String: \"%s\"\n", myrtx_string_cstr(from_cstr));
    printf("Starts with \"Hello\": %s\n", 
           myrtx_string_starts_with(from_cstr, "Hello") ? "Yes" : "No");
    printf("Ends with \"world!\": %s\n", 
           myrtx_string_ends_with(from_cstr, "world!") ? "Yes" : "No");
    
    /* Find substrings */
    size_t pos = myrtx_string_find(from_cstr, "world");
    printf("Position of \"world\": %zu\n", pos);
    
    /* Replace substrings */
    myrtx_string_t* for_replace = myrtx_string_from_cstr(&arena, "The quick brown fox jumps over the lazy dog");
    printf("Before replace: \"%s\"\n", myrtx_string_cstr(for_replace));
    myrtx_string_replace(for_replace, "fox", "cat");
    printf("After replace: \"%s\"\n\n", myrtx_string_cstr(for_replace));
    
    /* Split and join */
    printf("Split and join:\n");
    
    myrtx_string_t* to_split = myrtx_string_from_cstr(&arena, "apple,orange,banana,grape");
    printf("Original: \"%s\"\n", myrtx_string_cstr(to_split));
    
    size_t count = 0;
    myrtx_string_t* parts = myrtx_string_split(&arena, to_split, ",", &count);
    
    printf("Split into %zu parts:\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("  [%zu]: \"%s\"\n", i, myrtx_string_cstr(&parts[i]));
    }
    
    /* Join with a different delimiter */
    myrtx_string_t* joined = myrtx_string_join(&arena, parts, count, " + ");
    printf("Joined: \"%s\"\n\n", myrtx_string_cstr(joined));
    
    /* Trimming whitespace */
    printf("Trimming whitespace:\n");
    
    myrtx_string_t* with_whitespace = myrtx_string_from_cstr(&arena, "  \t  This has whitespace around it  \n  ");
    printf("Original: \"%s\"\n", myrtx_string_cstr(with_whitespace));
    myrtx_string_trim(with_whitespace);
    printf("Trimmed: \"%s\"\n\n", myrtx_string_cstr(with_whitespace));
    
    /* Using strings with scratch arenas */
    printf("Using with scratch arenas:\n");
    
    MYRTX_WITH_SCRATCH(&arena, scratch) {
        myrtx_string_t* temp = myrtx_string_from_cstr(scratch.arena, "This is a temporary string");
        myrtx_string_to_upper(temp);
        printf("Temporary string: \"%s\"\n", myrtx_string_cstr(temp));
        
        /* The string will be automatically freed with the scratch arena */
    }
    printf("After scratch arena scope\n\n");
    
    /* Memory management */
    myrtx_arena_free(&arena);
    
    printf("Example completed successfully\n");
    return 0;
} 