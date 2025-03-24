/**
 * @file string_example.c
 * @brief Example usage of myrtx string functions
 */

#include "myrtx/string/string.h"
#include <stdio.h>

/* Helper function to print a string array */
void print_string_array(const char** strings, size_t count) {
    printf("String array (%zu items):\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("  [%zu]: \"%s\"\n", i, strings[i]);
    }
    printf("\n");
}

int main(void) {
    printf("=== String Functions Example ===\n\n");
    
    /* Initialize arena */
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        printf("Failed to initialize arena\n");
        return 1;
    }
    
    /* Basic string duplication */
    printf("String duplication demo:\n");
    const char* original = "Hello, world!";
    char* duplicate = myrtx_strdup(&arena, original);
    printf("Original: \"%s\"\n", original);
    printf("Duplicate: \"%s\"\n\n", duplicate);
    
    /* Partial string duplication */
    printf("Partial string duplication demo:\n");
    char* partial = myrtx_strndup(&arena, "This is a long string", 7);
    printf("Result: \"%s\"\n\n", partial);
    
    /* String formatting */
    printf("String formatting demo:\n");
    char* formatted = myrtx_strfmt(&arena, "The value of %s is %d", "answer", 42);
    printf("Formatted: \"%s\"\n\n", formatted);
    
    /* String concatenation */
    printf("String concatenation demo:\n");
    char* concat = myrtx_strcat_dup(&arena, "Hello, ", "world!");
    printf("Concatenated: \"%s\"\n\n", concat);
    
    /* String splitting */
    printf("String splitting demo:\n");
    const char* to_split = "apple,orange,banana,grape";
    size_t token_count = 0;
    char** tokens = myrtx_strsplit(&arena, to_split, ",", &token_count);
    printf("Original: \"%s\"\n", to_split);
    print_string_array((const char**)tokens, token_count);
    
    /* String joining */
    printf("String joining demo:\n");
    const char* strings[] = {"apple", "orange", "banana", "grape"};
    char* joined = myrtx_strjoin(&arena, strings, 4, " + ");
    printf("Joined: \"%s\"\n\n", joined);
    
    /* Substring extraction */
    printf("Substring extraction demo:\n");
    const char* substr_source = "This is a test string";
    char* substr = myrtx_substr(&arena, substr_source, 5, 7);
    printf("Source: \"%s\"\n", substr_source);
    printf("Substring (5,7): \"%s\"\n\n", substr);
    
    /* Case conversion */
    printf("Case conversion demo:\n");
    const char* mixed_case = "This Is a MiXeD CaSe StRiNg";
    char* upper = myrtx_strtoupper(&arena, mixed_case);
    char* lower = myrtx_strtolower(&arena, mixed_case);
    printf("Original: \"%s\"\n", mixed_case);
    printf("Upper case: \"%s\"\n", upper);
    printf("Lower case: \"%s\"\n\n", lower);
    
    /* String prefix/suffix checking */
    printf("String prefix/suffix checking demo:\n");
    const char* prefix_test = "Hello, world!";
    printf("String: \"%s\"\n", prefix_test);
    printf("Starts with \"Hello\": %s\n", myrtx_startswith(prefix_test, "Hello") ? "Yes" : "No");
    printf("Starts with \"World\": %s\n", myrtx_startswith(prefix_test, "World") ? "Yes" : "No");
    printf("Ends with \"world!\": %s\n", myrtx_endswith(prefix_test, "world!") ? "Yes" : "No");
    printf("Ends with \"Hello\": %s\n\n", myrtx_endswith(prefix_test, "Hello") ? "Yes" : "No");
    
    /* String trimming */
    printf("String trimming demo:\n");
    const char* to_trim = "  \t  This has whitespace around it  \n  ";
    char* trimmed = myrtx_strtrim(&arena, to_trim);
    printf("Original: \"%s\"\n", to_trim);
    printf("Trimmed: \"%s\"\n\n", trimmed);
    
    /* String replacement */
    printf("String replacement demo:\n");
    const char* to_replace = "The quick brown fox jumps over the lazy dog";
    char* replaced = myrtx_strreplace(&arena, to_replace, "fox", "cat");
    printf("Original: \"%s\"\n", to_replace);
    printf("After replacement: \"%s\"\n\n", replaced);
    
    /* Nested function calls */
    printf("Nested function calls demo:\n");
    char* nested = myrtx_strtoupper(&arena, 
                       myrtx_strcat_dup(&arena,
                           "hello, ",
                           myrtx_substr(&arena, "universe and world", 0, 8)));
    printf("Result: \"%s\"\n\n", nested);
    
    /* Using with scratch arenas */
    printf("Using with scratch arenas demo:\n");
    MYRTX_WITH_SCRATCH(&arena, scratch) {
        char* temp1 = myrtx_strdup(scratch.arena, "This is a temporary string");
        char* temp2 = myrtx_strtoupper(scratch.arena, temp1);
        printf("Temporary result: \"%s\"\n", temp2);
        
        /* The temporary strings will be automatically freed */
    }
    printf("After scratch arena scope\n\n");
    
    /* Done - free the arena */
    myrtx_arena_free(&arena);
    
    printf("Example completed successfully\n");
    return 0;
} 