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
add_executable(string_example string_example.c)
target_link_libraries(string_example PRIVATE myrtx)
target_include_directories(string_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(string_type_example string_type_example.c)
target_link_libraries(string_type_example PRIVATE myrtx)
target_include_directories(string_type_example PRIVATE ${CMAKE_SOURCE_DIR}/include) 