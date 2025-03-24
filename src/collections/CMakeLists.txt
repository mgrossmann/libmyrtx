# Collections Paket

target_sources(myrtx
    PRIVATE
        hash_table.c
)

target_include_directories(myrtx
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
) 