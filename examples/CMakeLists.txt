# Example programs for myrtx
add_executable(arena_example arena_example.c)
target_link_libraries(arena_example PRIVATE myrtx)
target_include_directories(arena_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(scratch_example scratch_example.c)
target_link_libraries(scratch_example PRIVATE myrtx)
target_include_directories(scratch_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(context_example context_example.c)
target_link_libraries(context_example PRIVATE myrtx)
target_include_directories(context_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

# String examples
add_executable(string_utils_example string_utils_example.c)
target_link_libraries(string_utils_example PRIVATE myrtx)
target_include_directories(string_utils_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(string_example string_example.c)
target_link_libraries(string_example PRIVATE myrtx)
target_include_directories(string_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Collections examples
add_executable(hash_table_example hash_table_example.c)
target_link_libraries(hash_table_example PRIVATE myrtx)
target_include_directories(hash_table_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(avl_tree_example avl_tree_example.c)
target_link_libraries(avl_tree_example PRIVATE myrtx)
target_include_directories(avl_tree_example PRIVATE ${CMAKE_SOURCE_DIR}/include) 