# Testprogramme für myrtx
add_executable(arena_test arena_test.c)
target_link_libraries(arena_test PRIVATE myrtx)
target_include_directories(arena_test PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME arena_allocator_test COMMAND arena_test) 