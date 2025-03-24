cmake_minimum_required(VERSION 3.14)
project(myrtx 
        VERSION 0.1.0
        DESCRIPTION "MY Runtime eXtension - C99 Utility Library"
        LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Optionen
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(MYRTX_BUILD_TESTS "Build test programs" ON)
option(MYRTX_BUILD_EXAMPLES "Build example programs" ON)

# Bibliotheksquellen
set(MYRTX_SOURCES
    src/memory/arena_allocator.c
)

# Bibliothek erstellen
add_library(myrtx ${MYRTX_SOURCES})
target_include_directories(myrtx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Version in Header-Datei definieren
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/myrtx/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/myrtx/version.h
    @ONLY
)

# Installation konfigurieren
include(GNUInstallDirs)
install(TARGETS myrtx
    EXPORT myrtxTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Beispiele
if(MYRTX_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(MYRTX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
