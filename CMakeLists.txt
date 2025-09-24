cmake_minimum_required(VERSION 3.14)
project(myrtx C)

# Configure compiler flags
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Configure build options
option(MYRTX_BUILD_EXAMPLES "Build example programs" ON)
option(MYRTX_BUILD_TESTS "Build test programs" ON)

# Add debugging flags for debug builds
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -Wextra -Werror")

# Add optimization flags for release builds
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

# Setup include directories
include_directories(include)

# Add library sources
add_library(myrtx STATIC "")

# Add subdirectories
add_subdirectory(src)

# Add examples if enabled
if(MYRTX_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Add tests if enabled
if(MYRTX_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Install targets
install(TARGETS myrtx DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# Formatting helpers (clang-format)
find_program(CLANG_FORMAT NAMES clang-format)
if(CLANG_FORMAT)
  # Collect sources and headers to format
  file(GLOB_RECURSE MYRTX_FORMAT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/tests/*.c"
    "${CMAKE_SOURCE_DIR}/examples/*.c"
  )

  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i -style=file ${MYRTX_FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format on project sources"
  )

  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --dry-run -Werror -style=file ${MYRTX_FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking formatting (fails if changes are needed)"
  )
endif()
