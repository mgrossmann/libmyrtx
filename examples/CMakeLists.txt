# Beispielprogramme für myrtx
add_executable(arena_example arena_example.c)
target_link_libraries(arena_example PRIVATE myrtx)
target_include_directories(arena_example PRIVATE ${CMAKE_SOURCE_DIR}/include) 