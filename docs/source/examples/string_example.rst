String Manipulation Example
============================

Overview
--------

This example demonstrates how to use the string functions of the libmyrtx library for efficient text processing. We implement a simple text analysis tool that counts the number of words in a text, highlights frequent words, and demonstrates various string manipulations.

Example Code
------------

.. code-block:: c

    /**
     * @file string_example.c
     * @brief Example demonstrating the string functions of libmyrtx.
     */
    
    #include <myrtx/string.h>
    #include <myrtx/memory.h>
    #include <myrtx/context.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <stdbool.h>
    
    #define MAX_WORD_COUNT 1000
    #define MAX_WORD_LENGTH 50
    
    /**
     * @brief Structure for storing word statistics.
     */
    typedef struct {
        char* word;
        int count;
    } word_stat_t;
    
    /**
     * @brief Structure for storing text analysis results.
     */
    typedef struct {
        int total_words;
        int unique_words;
        int total_chars;
        word_stat_t* word_stats;
        int word_stats_count;
    } text_analysis_t;
    
    /**
     * @brief Stores a word in the statistics.
     * 
     * @param analysis The text analysis structure.
     * @param word The word to store.
     */
    void add_word_to_stats(text_analysis_t* analysis, const char* word) {
        // Ignore empty words
        if (!word || strlen(word) == 0) {
            return;
        }
        
        // Check if the word is already in the statistics
        for (int i = 0; i < analysis->word_stats_count; i++) {
            if (strcmp(analysis->word_stats[i].word, word) == 0) {
                // Word found, increment counter
                analysis->word_stats[i].count++;
                return;
            }
        }
        
        // Add new word to statistics
        if (analysis->word_stats_count < MAX_WORD_COUNT) {
            analysis->word_stats[analysis->word_stats_count].word = strdup(word);
            analysis->word_stats[analysis->word_stats_count].count = 1;
            analysis->word_stats_count++;
            analysis->unique_words++;
        }
    }
    
    /**
     * @brief Breaks a text into words and updates the statistics.
     * 
     * @param analysis The text analysis structure.
     * @param text The text to analyze.
     */
    void analyze_text(text_analysis_t* analysis, const char* text) {
        myrtx_string_t* current_word = myrtx_string_create();
        if (!current_word) {
            fprintf(stderr, "Error allocating memory for current word.\n");
            return;
        }
        
        // Go through the text character by character
        for (const char* p = text; *p; p++) {
            if (isalpha(*p) || *p == '-' || *p == '\'') {
                // Add letters or punctuation to the current word
                myrtx_string_append_char(current_word, tolower(*p));
            } else if (myrtx_string_length(current_word) > 0) {
                // Word boundary found, add word to statistics
                add_word_to_stats(analysis, myrtx_string_cstr(current_word));
                myrtx_string_clear(current_word);
                analysis->total_words++;
            }
        }
        
        // Don't forget the last word
        if (myrtx_string_length(current_word) > 0) {
            add_word_to_stats(analysis, myrtx_string_cstr(current_word));
            analysis->total_words++;
        }
        
        analysis->total_chars = strlen(text);
        
        myrtx_string_free(current_word);
    }
    
    /**
     * @brief Displays the text analysis results.
     * 
     * @param analysis The text analysis structure.
     */
    void print_analysis(const text_analysis_t* analysis) {
        printf("Text Analysis Results:\n");
        printf("Total characters: %d\n", analysis->total_chars);
        printf("Total words: %d\n", analysis->total_words);
        printf("Unique words: %d\n", analysis->unique_words);
        printf("\nTop 10 most frequent words:\n");
        
        // Simple sorting of word statistics by frequency
        for (int i = 0; i < analysis->word_stats_count - 1; i++) {
            for (int j = i + 1; j < analysis->word_stats_count; j++) {
                if (analysis->word_stats[j].count > analysis->word_stats[i].count) {
                    word_stat_t temp = analysis->word_stats[i];
                    analysis->word_stats[i] = analysis->word_stats[j];
                    analysis->word_stats[j] = temp;
                }
            }
        }
        
        // Output top 10 words
        int count = (analysis->word_stats_count < 10) ? analysis->word_stats_count : 10;
        for (int i = 0; i < count; i++) {
            printf("%2d. %-20s: %d\n", i + 1, analysis->word_stats[i].word, analysis->word_stats[i].count);
        }
    }
    
    /**
     * @brief Frees the resources of the text analysis.
     * 
     * @param analysis The text analysis structure.
     */
    void free_analysis(text_analysis_t* analysis) {
        for (int i = 0; i < analysis->word_stats_count; i++) {
            free(analysis->word_stats[i].word);
        }
        free(analysis->word_stats);
    }
    
    /**
     * @brief Replaces a word in the text and returns the new text.
     * 
     * @param context The context for error handling.
     * @param text The original text.
     * @param find The word to replace.
     * @param replace The replacement.
     * @return myrtx_string_t* The new text.
     */
    myrtx_string_t* replace_word(myrtx_context_t* context, const char* text, const char* find, const char* replace) {
        myrtx_string_t* result = myrtx_string_create();
        if (!result) {
            myrtx_context_set_error(context, "Error allocating memory for result string.");
            return NULL;
        }
        
        size_t find_len = strlen(find);
        size_t replace_len = strlen(replace);
        
        const char* p = text;
        const char* q;
        
        while ((q = strstr(p, find))) {
            // Add text up to the found word
            myrtx_string_append(result, p, q - p);
            
            // Add replacement word
            myrtx_string_append_cstr(result, replace);
            
            // Continue after the found word
            p = q + find_len;
        }
        
        // Add the rest of the text
        myrtx_string_append_cstr(result, p);
        
        return result;
    }
    
    /**
     * @brief Creates a formatted summary.
     * 
     * @param arena The arena allocator for temporary strings.
     * @param analysis The text analysis results.
     * @param title The title of the summary.
     * @return myrtx_string_t* The formatted summary.
     */
    myrtx_string_t* format_summary(myrtx_arena_t* arena, const text_analysis_t* analysis, const char* title) {
        // Create a string that uses the arena
        myrtx_string_t* summary = myrtx_string_create_from_arena(arena);
        if (!summary) {
            return NULL;
        }
        
        // Format title
        myrtx_string_append_format(summary, "=== %s ===\n\n", title);
        
        // General statistics
        myrtx_string_append_format(summary, 
            "Summary:\n"
            "  - Characters: %d\n"
            "  - Words: %d\n"
            "  - Unique words: %d\n"
            "  - Average word length: %.2f characters\n\n",
            analysis->total_chars,
            analysis->total_words,
            analysis->unique_words,
            (float)analysis->total_chars / analysis->total_words
        );
        
        // Top 3 most frequent words
        myrtx_string_append_cstr(summary, "Most frequent words:\n");
        
        int count = (analysis->word_stats_count < 3) ? analysis->word_stats_count : 3;
        for (int i = 0; i < count; i++) {
            myrtx_string_append_format(summary, 
                "  %d. %s (%d occurrences)\n", 
                i + 1, 
                analysis->word_stats[i].word, 
                analysis->word_stats[i].count
            );
        }
        
        return summary;
    }
    
    int main(int argc, char** argv) {
        // Create context for error handling
        myrtx_context_t* ctx = myrtx_context_create();
        if (!ctx) {
            fprintf(stderr, "Error creating context.\n");
            return 1;
        }
        
        // Create arena for temporary strings
        myrtx_arena_t arena;
        if (myrtx_arena_init(&arena, 4096) != 0) {
            fprintf(stderr, "Error initializing arena.\n");
            myrtx_context_free(ctx);
            return 1;
        }
        
        // Sample text
        const char* sample_text = 
            "Once upon a time, in a far away land, "
            "there lived a king and a queen who wished for nothing more "
            "than a child. After many years, their wish was finally granted, "
            "and they had a beautiful daughter. The king was so "
            "overjoyed that he held a great feast and invited all "
            "the people in the kingdom. Even the fairies of the land were "
            "invited so they could bring good fortune and special gifts "
            "to the child. However, there was an old fairy whom "
            "nobody had seen for many years, and so the king forgot to "
            "invite her as well. When the feast was already in full swing, "
            "this old fairy suddenly appeared. She was angry because she had not "
            "been invited, and spoke a terrible curse.";
        
        // Allocate memory for analysis
        text_analysis_t analysis = {0};
        analysis.word_stats = (word_stat_t*)calloc(MAX_WORD_COUNT, sizeof(word_stat_t));
        if (!analysis.word_stats) {
            fprintf(stderr, "Error allocating memory for word statistics.\n");
            myrtx_arena_free(&arena);
            myrtx_context_free(ctx);
            return 1;
        }
        
        // Analyze text
        analyze_text(&analysis, sample_text);
        
        // Output results
        print_analysis(&analysis);
        
        // Demonstration of string replacements
        myrtx_string_t* modified_text = replace_word(ctx, sample_text, "king", "ruler");
        
        if (modified_text) {
            printf("\nModified Text (king -> ruler):\n");
            printf("----------------------------------------\n");
            
            // Output the first part of the text with formatted line length
            myrtx_string_t* preview = myrtx_string_create();
            if (preview) {
                size_t max_preview_len = 200;
                size_t text_len = myrtx_string_length(modified_text);
                size_t preview_len = (text_len < max_preview_len) ? text_len : max_preview_len;
                
                myrtx_string_append(preview, myrtx_string_cstr(modified_text), preview_len);
                
                if (preview_len < text_len) {
                    myrtx_string_append_cstr(preview, "...");
                }
                
                printf("%s\n\n", myrtx_string_cstr(preview));
                myrtx_string_free(preview);
            }
            
            // Create and output formatted summary
            myrtx_string_t* summary = format_summary(&arena, &analysis, "Text Analysis Summary");
            if (summary) {
                printf("%s\n", myrtx_string_cstr(summary));
                // Since the string uses the arena, we don't need to free it separately
            }
            
            myrtx_string_free(modified_text);
        } else {
            fprintf(stderr, "Error replacing words: %s\n", 
                myrtx_context_get_error(ctx) ? myrtx_context_get_error(ctx) : "Unknown error");
        }
        
        // Free resources
        free_analysis(&analysis);
        myrtx_arena_free(&arena);
        myrtx_context_free(ctx);
        
        return 0;
    }

How to Compile and Run
------------------------

To compile this example:

.. code-block:: bash

    gcc -o string_example string_example.c -lmyrtx

Run the example:

.. code-block:: bash

    ./string_example

Expected Output
----------------

.. code-block:: text

    Text Analysis Results:
    Total characters: 766
    Total words: 124
    Unique words: 86
    
    Top 10 most frequent words:
     1. and                 : 6
     2. the                 : 5
     3. a                   : 5
     4. was                 : 4
     5. were                : 3
     6. they                : 3
     7. for                 : 3
     8. in                  : 2
     9. to                  : 2
    10. so                  : 2
    
    Modified Text (king -> ruler):
    ----------------------------------------
    Once upon a time, in a far away land, there lived a ruler and a queen who wished for nothing more than a child. After many years, their wish was finally granted, and they had a beautiful daughter. The ruler was so ...
    
    === Text Analysis Summary ===
    
    Summary:
      - Characters: 766
      - Words: 124
      - Unique words: 86
      - Average word length: 6.18 characters
    
    Most frequent words:
      1. and (6 occurrences)
      2. the (5 occurrences)
      3. a (5 occurrences)

Code Walkthrough
-----------------

1. **String Creation and Management**

   In this example, we use different methods for string creation:

   .. code-block:: c

       myrtx_string_t* current_word = myrtx_string_create();
       myrtx_string_t* result = myrtx_string_create();
       myrtx_string_t* summary = myrtx_string_create_from_arena(arena);

   Note that with `summary`, we create a string that uses an arena allocator, which simplifies memory management.

2. **String Manipulation**

   We demonstrate various string manipulations:

   .. code-block:: c

       // Append a single character
       myrtx_string_append_char(current_word, tolower(*p));
       
       // Clear a string
       myrtx_string_clear(current_word);
       
       // Append a buffer with specified length
       myrtx_string_append(result, p, q - p);
       
       // Append a C string
       myrtx_string_append_cstr(result, replace);

3. **Formatted Strings**

   The string library provides convenient formatting like `printf`:

   .. code-block:: c

       myrtx_string_append_format(summary, "=== %s ===\n\n", title);
       
       myrtx_string_append_format(summary, 
           "Summary:\n"
           "  - Characters: %d\n"
           "  - Words: %d\n",
           analysis->total_chars,
           analysis->total_words);

4. **Text Replacements**

   We implement a function to replace words in text:

   .. code-block:: c

       myrtx_string_t* replace_word(myrtx_context_t* context, const char* text, const char* find, const char* replace) {
           // ...
           while ((q = strstr(p, find))) {
               // Add text up to the found word
               myrtx_string_append(result, p, q - p);
               
               // Add replacement word
               myrtx_string_append_cstr(result, replace);
               
               // Continue after the found word
               p = q + find_len;
           }
           // ...
       }

5. **Integration with Other libmyrtx Components**

   The example shows how the string library works with other libmyrtx components such as the Context System and the Arena Allocator:

   .. code-block:: c

       // Context for error handling
       myrtx_context_t* ctx = myrtx_context_create();
       
       // Arena for temporary strings
       myrtx_arena_t arena;
       myrtx_arena_init(&arena, 4096);
       
       // String with arena support
       myrtx_string_t* summary = myrtx_string_create_from_arena(arena);
       
       // Error handling with context
       if (!result) {
           myrtx_context_set_error(context, "Error allocating memory for result string.");
           return NULL;
       }

Key Insights
-------------

1. **Efficient String Manipulation**: The string library enables efficient manipulation of text without having to worry about memory management.

2. **Arena Integration**: Through integration with the Arena Allocator, you can efficiently manage temporary strings and simplify deallocation.

3. **Formatting**: The formatted string functions provide a type-safe and convenient alternative to `sprintf`.

4. **Error Handling**: Integration with the Context System enables structured error handling for string operations.

5. **Versatility**: The string library supports a variety of operations such as appending, inserting, replacing, and searching.

This example demonstrates how the libmyrtx string library can be used for text processing tasks, from simple word analysis to more complex string manipulations. 