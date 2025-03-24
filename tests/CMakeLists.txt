# Test programs for myrtx
add_executable(arena_test arena_test.c)
target_link_libraries(arena_test PRIVATE myrtx)
target_include_directories(arena_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(context_test context_test.c)
target_link_libraries(context_test PRIVATE myrtx)
target_include_directories(context_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME arena_allocator_test COMMAND arena_test)
add_test(NAME context_system_test COMMAND context_test) 