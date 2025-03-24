/**
 * @file string_type_test.c
 * @brief Tests for the myrtx string type and functions
 */

#include "myrtx/string/string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_PASSED() printf("PASSED: %s\n", __func__)
#define TEST_FAILED(msg) do { printf("FAILED: %s - %s\n", __func__, msg); exit(1); } while(0)

/* Test string creation functions */
void test_string_create(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test creating an empty string */
    myrtx_string_t empty = myrtx_string_create(&arena, 10);
    if (!empty.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create empty string");
    }
    
    if (empty.length != 0 || empty.capacity < 10 || strcmp(empty.data, "") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Empty string has incorrect values");
    }
    
    /* Test creating from C string */
    const char* original = "Test string for creation";
    myrtx_string_t from_cstr = myrtx_string_from_cstr(&arena, original);
    if (!from_cstr.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string from C string");
    }
    
    if (from_cstr.length != strlen(original) || 
        from_cstr.capacity != strlen(original) + 1 || 
        strcmp(from_cstr.data, original) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String from C string has incorrect values");
    }
    
    /* Test creating from buffer with explicit length */
    const char buffer[] = {'T', 'e', 's', 't', '\0', 'X', 'Y', 'Z'};
    size_t buffer_len = sizeof(buffer);
    myrtx_string_t from_buffer = myrtx_string_from_buffer(&arena, buffer, buffer_len);
    if (!from_buffer.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string from buffer");
    }
    
    if (from_buffer.length != buffer_len || 
        from_buffer.capacity != buffer_len + 1 || 
        memcmp(from_buffer.data, buffer, buffer_len) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String from buffer has incorrect values");
    }
    
    /* Test creating with formatting */
    int num = 42;
    const char* formatted_expected = "The answer is 42";
    myrtx_string_t formatted = myrtx_string_format(&arena, "The answer is %d", num);
    if (!formatted.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create formatted string");
    }
    
    if (formatted.length != strlen(formatted_expected) || 
        strcmp(formatted.data, formatted_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Formatted string has incorrect values");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test string modification functions */
void test_string_modify(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test setting string content */
    myrtx_string_t str = myrtx_string_create(&arena, 16);
    if (!str.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string");
    }
    
    const char* content = "Test content";
    if (!myrtx_string_set(&str, content)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to set string content");
    }
    
    if (str.length != strlen(content) || strcmp(str.data, content) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String set resulted in incorrect values");
    }
    
    /* Test setting content beyond initial capacity (should trigger resize) */
    const char* long_content = "This is a much longer string that should trigger a resize of the string buffer";
    if (!myrtx_string_set(&str, long_content)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to set longer string content");
    }
    
    if (str.length != strlen(long_content) || strcmp(str.data, long_content) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String set with long content resulted in incorrect values");
    }
    
    /* Test appending to string */
    myrtx_string_t append_str = myrtx_string_from_cstr(&arena, "Hello");
    if (!append_str.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string for append test");
    }
    
    const char* append_text = ", world!";
    if (!myrtx_string_append(&append_str, append_text)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to append to string");
    }
    
    const char* append_expected = "Hello, world!";
    if (append_str.length != strlen(append_expected) || 
        strcmp(append_str.data, append_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String append resulted in incorrect values");
    }
    
    /* Test append formatting */
    if (!myrtx_string_append_format(&append_str, " (%d)", 2023)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to append formatted text");
    }
    
    const char* append_format_expected = "Hello, world! (2023)";
    if (append_str.length != strlen(append_format_expected) || 
        strcmp(append_str.data, append_format_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String append with formatting resulted in incorrect values");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test string manipulation functions */
void test_string_manipulate(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test substring extraction */
    const char* source = "This is a test string";
    myrtx_string_t str = myrtx_string_from_cstr(&arena, source);
    if (!str.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string for substring test");
    }
    
    myrtx_string_t substr = myrtx_string_substr(&arena, &str, 5, 7);
    if (!substr.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to extract substring");
    }
    
    const char* substr_expected = "is a te";
    if (substr.length != strlen(substr_expected) || 
        strcmp(substr.data, substr_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Substring extraction resulted in incorrect values");
    }
    
    /* Test clone */
    myrtx_string_t clone = myrtx_string_clone(&arena, &str);
    if (!clone.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to clone string");
    }
    
    if (clone.length != str.length || 
        clone.capacity != str.capacity || 
        strcmp(clone.data, str.data) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String clone resulted in incorrect values");
    }
    
    /* Test upper/lower case conversion */
    myrtx_string_t mixed_case = myrtx_string_from_cstr(&arena, "MiXeD CaSe StRiNg");
    if (!mixed_case.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string for case conversion test");
    }
    
    if (!myrtx_string_to_upper(&mixed_case)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to convert to uppercase");
    }
    
    const char* upper_expected = "MIXED CASE STRING";
    if (strcmp(mixed_case.data, upper_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Uppercase conversion resulted in incorrect values");
    }
    
    if (!myrtx_string_to_lower(&mixed_case)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to convert to lowercase");
    }
    
    const char* lower_expected = "mixed case string";
    if (strcmp(mixed_case.data, lower_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Lowercase conversion resulted in incorrect values");
    }
    
    /* Test trimming */
    myrtx_string_t to_trim = myrtx_string_from_cstr(&arena, "  \t  Trim me  \n  ");
    if (!to_trim.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string for trim test");
    }
    
    if (!myrtx_string_trim(&to_trim)) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to trim string");
    }
    
    const char* trim_expected = "Trim me";
    if (to_trim.length != strlen(trim_expected) || 
        strcmp(to_trim.data, trim_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String trim resulted in incorrect values");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test string operations */
void test_string_operations(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test comparison */
    myrtx_string_t str1 = myrtx_string_from_cstr(&arena, "String A");
    myrtx_string_t str2 = myrtx_string_from_cstr(&arena, "String B");
    myrtx_string_t str3 = myrtx_string_from_cstr(&arena, "String A");
    
    if (myrtx_string_compare(&str1, &str2) >= 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String comparison (less than) failed");
    }
    
    if (myrtx_string_compare(&str2, &str1) <= 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String comparison (greater than) failed");
    }
    
    if (myrtx_string_compare(&str1, &str3) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String comparison (equal) failed");
    }
    
    /* Test starts/ends with */
    myrtx_string_t prefix_suffix = myrtx_string_from_cstr(&arena, "Hello, world!");
    
    if (!myrtx_string_starts_with(&prefix_suffix, "Hello")) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String starts_with failed for valid prefix");
    }
    
    if (myrtx_string_starts_with(&prefix_suffix, "hello")) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String starts_with should be case sensitive");
    }
    
    if (!myrtx_string_ends_with(&prefix_suffix, "world!")) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String ends_with failed for valid suffix");
    }
    
    if (myrtx_string_ends_with(&prefix_suffix, "World!")) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String ends_with should be case sensitive");
    }
    
    /* Test find/rfind */
    myrtx_string_t haystack = myrtx_string_from_cstr(&arena, "one two three two one");
    
    size_t pos = myrtx_string_find(&haystack, "two");
    if (pos != 4) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String find returned incorrect position");
    }
    
    pos = myrtx_string_find_from(&haystack, "two", 5);
    if (pos != 14) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String find_from returned incorrect position");
    }
    
    pos = myrtx_string_rfind(&haystack, "one");
    if (pos != 18) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String rfind returned incorrect position");
    }
    
    /* Test replace */
    myrtx_string_t to_replace = myrtx_string_from_cstr(&arena, "The quick brown fox jumps over the lazy dog");
    
    if (!myrtx_string_replace(&to_replace, "fox", "cat")) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String replace failed");
    }
    
    const char* replace_expected = "The quick brown cat jumps over the lazy dog";
    if (strcmp(myrtx_string_cstr(&to_replace), replace_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String replace resulted in incorrect values");
    }
    
    /* Test multiple replacements */
    myrtx_string_t multi_replace = myrtx_string_from_cstr(&arena, "one two one two one");
    
    if (!myrtx_string_replace(&multi_replace, "one", "three")) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String multiple replace failed");
    }
    
    const char* multi_replace_expected = "three two three two three";
    if (strcmp(myrtx_string_cstr(&multi_replace), multi_replace_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("String multiple replace resulted in incorrect values");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test split and join */
void test_string_split_join(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    /* Test basic splitting */
    myrtx_string_t to_split = myrtx_string_from_cstr(&arena, "apple,orange,banana,grape");
    if (!to_split.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to create string for split test");
    }
    
    size_t count = 0;
    myrtx_string_t* parts = myrtx_string_split(&arena, &to_split, ",", &count);
    
    if (!parts) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to split string");
    }
    
    if (count != 4) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Split returned incorrect number of parts");
    }
    
    if (strcmp(myrtx_string_cstr(&parts[0]), "apple") != 0 ||
        strcmp(myrtx_string_cstr(&parts[1]), "orange") != 0 ||
        strcmp(myrtx_string_cstr(&parts[2]), "banana") != 0 ||
        strcmp(myrtx_string_cstr(&parts[3]), "grape") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Split parts have incorrect values");
    }
    
    /* Test splitting with empty parts */
    myrtx_string_t with_empty = myrtx_string_from_cstr(&arena, "one,,three");
    size_t empty_count = 0;
    myrtx_string_t* empty_parts = myrtx_string_split(&arena, &with_empty, ",", &empty_count);
    
    if (!empty_parts) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to split string with empty parts");
    }
    
    if (empty_count != 3) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Split with empty parts returned incorrect number of parts");
    }
    
    if (strcmp(myrtx_string_cstr(&empty_parts[0]), "one") != 0 ||
        strcmp(myrtx_string_cstr(&empty_parts[1]), "") != 0 ||
        strcmp(myrtx_string_cstr(&empty_parts[2]), "three") != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Split parts with empty parts have incorrect values");
    }
    
    /* Test joining */
    myrtx_string_t joined = myrtx_string_join(&arena, parts, count, ", ");
    if (!joined.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to join strings");
    }
    
    const char* join_expected = "apple, orange, banana, grape";
    if (joined.length != strlen(join_expected) || 
        strcmp(joined.data, join_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Joined string has incorrect value");
    }
    
    /* Test joining with empty delimiter */
    myrtx_string_t joined_no_delim = myrtx_string_join(&arena, parts, count, "");
    if (!joined_no_delim.data) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Failed to join strings with empty delimiter");
    }
    
    const char* join_no_delim_expected = "appleorangebananagrape";
    if (strcmp(joined_no_delim.data, join_no_delim_expected) != 0) {
        myrtx_arena_free(&arena);
        TEST_FAILED("Joined string with empty delimiter has incorrect value");
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

/* Test with scratch arenas */
void test_string_with_scratch(void) {
    myrtx_arena_t arena = {0};
    if (!myrtx_arena_init(&arena, 0)) {
        TEST_FAILED("Failed to initialize arena");
    }
    
    MYRTX_WITH_SCRATCH(&arena, scratch) {
        /* Create a string in the scratch arena */
        myrtx_string_t temp = myrtx_string_from_cstr(scratch.arena, "Temporary string");
        if (!temp.data) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to create string in scratch arena");
        }
        
        /* Modify the string */
        if (!myrtx_string_append(&temp, " with more text")) {
            myrtx_arena_free(&arena);
            TEST_FAILED("Failed to append to string in scratch arena");
        }
        
        const char* expected = "Temporary string with more text";
        if (strcmp(temp.data, expected) != 0) {
            myrtx_arena_free(&arena);
            TEST_FAILED("String in scratch arena has incorrect value");
        }
        
        /* Scratch arena will be freed automatically */
    }
    
    myrtx_arena_free(&arena);
    TEST_PASSED();
}

int main(void) {
    printf("=== myrtx String Type Tests ===\n\n");
    
    test_string_create();
    test_string_modify();
    test_string_manipulate();
    test_string_operations();
    test_string_split_join();
    test_string_with_scratch();
    
    printf("\nAll string type tests successful!\n");
    return 0;
} 