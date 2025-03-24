# Collections Paket

target_sources(myrtx
    PRIVATE
        hash_table.c
        avl_tree.c
)

target_include_directories(myrtx
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
) 