# Test programs for myrtx
add_executable(arena_test arena_test.c)
target_link_libraries(arena_test PRIVATE myrtx)
target_include_directories(arena_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(context_test context_test.c)
target_link_libraries(context_test PRIVATE myrtx)
target_include_directories(context_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(string_utils_test string_utils_test.c)
target_link_libraries(string_utils_test PRIVATE myrtx)
target_include_directories(string_utils_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(string_test string_test.c)
target_link_libraries(string_test PRIVATE myrtx)
target_include_directories(string_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(hash_table_test hash_table_test.c)
target_link_libraries(hash_table_test PRIVATE myrtx)
target_include_directories(hash_table_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(avl_tree_test avl_tree_test.c)
target_link_libraries(avl_tree_test PRIVATE myrtx)
target_include_directories(avl_tree_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME arena_allocator_test COMMAND arena_test)
add_test(NAME context_system_test COMMAND context_test)
add_test(NAME string_utils_test COMMAND string_utils_test)
add_test(NAME string_test COMMAND string_test)
add_test(NAME hash_table_test COMMAND hash_table_test)
add_test(NAME avl_tree_test COMMAND avl_tree_test) 