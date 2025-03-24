Getting Started with libmyrtx
==========================

This page will guide you through the initial steps of setting up and using libmyrtx in your projects.

Prerequisites
------------

Before you begin, ensure you have the following requirements:

- C99-compliant compiler (GCC, Clang, MSVC, etc.)
- CMake 3.14 or higher
- Git (for obtaining the source code)

Building from Source
------------------

1. Clone the repository:

   .. code-block:: bash

      git clone https://github.com/your-username/libmyrtx.git
      cd libmyrtx

2. Create a build directory and build the library:

   .. code-block:: bash

      mkdir build && cd build
      cmake ..
      cmake --build .

3. (Optional) Run tests to verify the build:

   .. code-block:: bash

      ctest

4. (Optional) Install the library to your system:

   .. code-block:: bash

      cmake --install .

Using the Build Script
--------------------

For convenience, a build script is provided:

.. code-block:: bash

   ./build.sh

This will use the default settings. For more options:

.. code-block:: bash

   ./build.sh --help

Example output:

.. code-block:: bash

   Usage: ./build.sh [options]
   Options:
     --type <Debug|Release|RelWithDebInfo|MinSizeRel>  Set build type (default: Debug)
     --clean                                          Clean build directory
     --test                                           Run tests after build
     --help                                           Show this help message

Integration into Your Project
---------------------------

There are several ways to integrate libmyrtx into your project:

Using CMake
~~~~~~~~~~

If your project uses CMake, you can add libmyrtx as a subdirectory:

.. code-block:: cmake

   add_subdirectory(path/to/libmyrtx)
   target_link_libraries(YourProject PRIVATE myrtx)

Using as an Installed Library
~~~~~~~~~~~~~~~~~~~~~~~~~~~

If libmyrtx is installed on your system:

.. code-block:: cmake

   find_package(myrtx REQUIRED)
   target_link_libraries(YourProject PRIVATE myrtx)

Manual Integration
~~~~~~~~~~~~~~~

For projects not using CMake, you can:

1. Build libmyrtx as described above
2. Copy the resulting static or shared library (`libmyrtx.a` or `libmyrtx.so`) to your project
3. Include the headers from `include/myrtx/`
4. Link against the library during compilation

Basic Usage Example
-----------------

Here's a simple example to get you started:

.. code-block:: c

   #include "myrtx/myrtx.h"
   #include <stdio.h>

   int main(void) {
       // Initialize arena allocator
       myrtx_arena_t arena = {0};
       if (!myrtx_arena_init(&arena, 0)) {
           printf("Failed to initialize arena\n");
           return 1;
       }

       // Allocate memory
       char* buffer = (char*)myrtx_arena_alloc(&arena, 1024);
       if (!buffer) {
           printf("Memory allocation failed\n");
           return 1;
       }

       // Use the memory
       sprintf(buffer, "Hello, libmyrtx!");
       printf("%s\n", buffer);

       // Clean up
       myrtx_arena_free(&arena);
       return 0;
   }

Next Steps
---------

Explore the detailed guides and API documentation to learn more about specific features:

- :doc:`guides/index` - In-depth guides on using various features
- :doc:`api/index` - Detailed API reference
- :doc:`examples/index` - Example code demonstrating different features 