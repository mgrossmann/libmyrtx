/**
 * @file string_utils_test.c
 * @brief Tests for myrtx string functions
 */

#include "myrtx/string/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_PASSED() printf("PASSED: %s\n", __func__)
#define TEST_FAILED(msg) do { printf("FAILED: %s - %s\n", __func__, msg); exit(1); } while(0)

/* Test strdup function */
void test_strdup(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    const char* original = "Test string for duplication";
    char* duplicate = myrtx_strdup(&arena, original);
    
    if (!duplicate) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to duplicate string");
    }
    
    if (strcmp(original, duplicate) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Duplicated string does not match original");
    }
    
    /* Check if the duplicate is actually a different string in memory */
    if (original == duplicate) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Duplicate points to the same memory as original");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strndup function */
void test_strndup(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    const char* original = "Test string for partial duplication";
    
    /* Test partial duplication */
    char* partial = myrtx_strndup(&arena, original, 11);
    if (!partial) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to partially duplicate string");
    }
    
    if (strcmp(partial, "Test string") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Partially duplicated string does not match expected result");
    }
    
    /* Test with length longer than string */
    char* full = myrtx_strndup(&arena, original, 100);
    if (!full) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to duplicate string with length > strlen");
    }
    
    if (strcmp(original, full) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String duplicated with length > strlen does not match original");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strfmt function */
void test_strfmt(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic formatting */
    char* basic = myrtx_strfmt(&arena, "The answer is %d", 42);
    if (!basic) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to format basic string");
    }
    
    if (strcmp(basic, "The answer is 42") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Formatted string does not match expected result");
    }
    
    /* Test multiple arguments */
    char* multi = myrtx_strfmt(&arena, "%s %d %c %.2f", "Test", 123, 'X', 3.14159);
    if (!multi) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to format string with multiple arguments");
    }
    
    if (strcmp(multi, "Test 123 X 3.14") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Formatted string with multiple arguments does not match expected result");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strcat_dup function */
void test_strcat_dup(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic concatenation */
    const char* first = "Hello, ";
    const char* second = "world!";
    char* result = myrtx_strcat_dup(&arena, first, second);
    
    if (!result) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to concatenate strings");
    }
    
    if (strcmp(result, "Hello, world!") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Concatenated string does not match expected result");
    }
    
    /* Test with empty strings */
    char* empty1 = myrtx_strcat_dup(&arena, "", second);
    if (!empty1 || strcmp(empty1, second) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Concatenation with empty first string failed");
    }
    
    char* empty2 = myrtx_strcat_dup(&arena, first, "");
    if (!empty2 || strcmp(empty2, first) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Concatenation with empty second string failed");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strsplit function */
void test_strsplit(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic splitting */
    const char* to_split = "apple,orange,banana,grape";
    size_t token_count = 0;
    char** tokens = myrtx_strsplit(&arena, to_split, ",", &token_count);
    
    if (!tokens) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to split string");
    }
    
    if (token_count != 4) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Incorrect token count");
    }
    
    if (strcmp(tokens[0], "apple") != 0 ||
        strcmp(tokens[1], "orange") != 0 ||
        strcmp(tokens[2], "banana") != 0 ||
        strcmp(tokens[3], "grape") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Split tokens do not match expected values");
    }
    
    /* Test with empty parts */
    const char* with_empty = "one,,three";
    size_t empty_count = 0;
    char** empty_tokens = myrtx_strsplit(&arena, with_empty, ",", &empty_count);
    
    if (!empty_tokens) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to split string with empty parts");
    }
    
    if (empty_count != 3) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Incorrect token count for string with empty parts");
    }
    
    if (strcmp(empty_tokens[0], "one") != 0 ||
        strcmp(empty_tokens[1], "") != 0 ||
        strcmp(empty_tokens[2], "three") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Split tokens with empty parts do not match expected values");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strjoin function */
void test_strjoin(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic joining */
    const char* strings[] = {"apple", "orange", "banana", "grape"};
    char* joined = myrtx_strjoin(&arena, strings, 4, ", ");
    
    if (!joined) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to join strings");
    }
    
    if (strcmp(joined, "apple, orange, banana, grape") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Joined string does not match expected result");
    }
    
    /* Test with empty separator */
    char* no_sep = myrtx_strjoin(&arena, strings, 4, "");
    if (!no_sep) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to join strings with empty separator");
    }
    
    if (strcmp(no_sep, "appleorangebananagrape") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Joined string with empty separator does not match expected result");
    }
    
    /* Test with single string */
    char* single = myrtx_strjoin(&arena, strings, 1, ", ");
    if (!single) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to join single string");
    }
    
    if (strcmp(single, "apple") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Joined single string does not match expected result");
    }
    
    /* Test with no strings */
    char* none = myrtx_strjoin(&arena, strings, 0, ", ");
    if (!none) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to join no strings");
    }
    
    if (strcmp(none, "") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Joined no strings does not match expected result");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test substr function */
void test_substr(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    const char* source = "This is a test string";
    
    /* Test basic substring */
    char* basic = myrtx_substr(&arena, source, 5, 7);
    if (!basic) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to extract basic substring");
    }
    
    if (strcmp(basic, "is a te") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Basic substring does not match expected result");
    }
    
    /* Test substring from beginning */
    char* start = myrtx_substr(&arena, source, 0, 4);
    if (!start) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to extract substring from beginning");
    }
    
    if (strcmp(start, "This") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Substring from beginning does not match expected result");
    }
    
    /* Test substring to end */
    char* end = myrtx_substr(&arena, source, 10, 100); /* Length beyond end of string */
    if (!end) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to extract substring to end");
    }
    
    if (strcmp(end, "test string") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Substring to end does not match expected result");
    }
    
    /* Test invalid start position */
    char* invalid = myrtx_substr(&arena, source, 100, 5);
    if (!invalid) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to handle invalid start position");
    }
    
    if (strcmp(invalid, "") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Result of invalid start position does not match expected result");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strtoupper and strtolower functions */
void test_case_conversion(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    const char* mixed = "This Is a MiXeD CaSe StRiNg";
    
    /* Test uppercase conversion */
    char* upper = myrtx_strtoupper(&arena, mixed);
    if (!upper) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to convert string to uppercase");
    }
    
    if (strcmp(upper, "THIS IS A MIXED CASE STRING") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Uppercase conversion result does not match expected result");
    }
    
    /* Test lowercase conversion */
    char* lower = myrtx_strtolower(&arena, mixed);
    if (!lower) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to convert string to lowercase");
    }
    
    if (strcmp(lower, "this is a mixed case string") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Lowercase conversion result does not match expected result");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test startswith and endswith functions */
void test_starts_ends_with(void) {
    const char* test_str = "Hello, world!";
    
    /* Test startswith */
    if (!myrtx_startswith(test_str, "Hello")) {
        TEST_FAILED("startswith failed to match valid prefix");
    }
    
    if (myrtx_startswith(test_str, "hello")) { /* Case sensitive */
        TEST_FAILED("startswith incorrectly matched with different case");
    }
    
    if (myrtx_startswith(test_str, "Goodbye")) {
        TEST_FAILED("startswith incorrectly matched invalid prefix");
    }
    
    if (myrtx_startswith(test_str, "Hello, world! And more")) {
        TEST_FAILED("startswith incorrectly matched prefix longer than string");
    }
    
    /* Test endswith */
    if (!myrtx_endswith(test_str, "world!")) {
        TEST_FAILED("endswith failed to match valid suffix");
    }
    
    if (myrtx_endswith(test_str, "World!")) { /* Case sensitive */
        TEST_FAILED("endswith incorrectly matched with different case");
    }
    
    if (myrtx_endswith(test_str, "universe!")) {
        TEST_FAILED("endswith incorrectly matched invalid suffix");
    }
    
    if (myrtx_endswith(test_str, "More text and Hello, world!")) {
        TEST_FAILED("endswith incorrectly matched suffix longer than string");
    }
    
    TEST_PASSED();
}

/* Test strtrim function */
void test_strtrim(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic trimming */
    const char* spaces = "  \t  This has whitespace around it  \n  ";
    char* trimmed = myrtx_strtrim(&arena, spaces);
    if (!trimmed) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to trim string");
    }
    
    if (strcmp(trimmed, "This has whitespace around it") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Trimmed string does not match expected result");
    }
    
    /* Test with no whitespace */
    const char* no_spaces = "NoWhitespace";
    char* no_trim = myrtx_strtrim(&arena, no_spaces);
    if (!no_trim) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to trim string with no whitespace");
    }
    
    if (strcmp(no_trim, "NoWhitespace") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Trimmed string with no whitespace does not match expected result");
    }
    
    /* Test with only whitespace */
    const char* only_spaces = "  \t\n  ";
    char* empty = myrtx_strtrim(&arena, only_spaces);
    if (!empty) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to trim string with only whitespace");
    }
    
    if (strcmp(empty, "") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Trimmed string with only whitespace does not match expected result");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test strreplace function */
void test_strreplace(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic replacement */
    const char* original = "The quick brown fox jumps over the lazy dog";
    char* replaced = myrtx_strreplace(&arena, original, "fox", "cat");
    if (!replaced) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to replace substring");
    }
    
    if (strcmp(replaced, "The quick brown cat jumps over the lazy dog") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Replaced string does not match expected result");
    }
    
    /* Test multiple replacements */
    char* multi = myrtx_strreplace(&arena, "one two one two one", "one", "three");
    if (!multi) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to replace multiple occurrences");
    }
    
    if (strcmp(multi, "three two three two three") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Multiple replacements do not match expected result");
    }
    
    /* Test replacement with different length */
    char* diff_len = myrtx_strreplace(&arena, "replace this word", "word", "phrase that is longer");
    if (!diff_len) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to replace with different length");
    }
    
    if (strcmp(diff_len, "replace this phrase that is longer") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Replacement with different length does not match expected result");
    }
    
    /* Test no matches */
    char* no_match = myrtx_strreplace(&arena, "No matches here", "xyz", "abc");
    if (!no_match) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to handle no matches");
    }
    
    if (strcmp(no_match, "No matches here") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Result with no matches does not match expected result");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test with scratch arenas */
void test_with_scratch(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test using scratch arena for temporary allocations */
    bool success = false;
    
    MYRTX_WITH_SCRATCH(&arena, scratch) {
        char* temp1 = myrtx_strdup(scratch.arena, "This is a temporary string");
        if (!temp1) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to allocate temporary string in scratch arena");
        }
        
        char* temp2 = myrtx_strtoupper(scratch.arena, temp1);
        if (!temp2) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to convert temporary string to uppercase");
        }
        
        if (strcmp(temp2, "THIS IS A TEMPORARY STRING") != 0) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Temporary string conversion does not match expected result");
        }
        
        success = true;
    }
    
    if (!success) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Scratch arena block did not complete");
    }
    
    /* The memory should be automatically freed by the MYRTX_WITH_SCRATCH macro */
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

int main(void) {
    printf("=== myrtx String Utils Tests ===\n\n");
    
    test_strdup();
    test_strndup();
    test_strfmt();
    test_strcat_dup();
    test_strsplit();
    test_strjoin();
    test_substr();
    test_case_conversion();
    test_starts_ends_with();
    test_strtrim();
    test_strreplace();
    test_with_scratch();
    
    printf("\nAll string utils tests successful!\n");
    return 0;
} 