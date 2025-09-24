Welcome to libmyrtx's documentation!
====================================

.. only:: html

   .. image:: https://readthedocs.org/projects/libmyrtx/badge/?version=latest
      :target: https://libmyrtx.readthedocs.io/en/latest/?badge=latest
      :alt: Documentation Status

libmyrtx is a C99 library that provides useful functions and data structures for use in various C projects.
The library focuses on memory allocators, strings, and other fundamental data structures.

Features
--------

- **Arena Allocator**: A fast, region-based memory allocator
- **Context System**: Context management and error handling
- **String Functions**: Utility functions for string manipulation
- **String Type**: Dynamic string type for easier string handling
- **Hash Table**: High-performance hash table implementation
- **AVL Tree**: Self-balancing binary search tree

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   getting_started
   guides/index
   api/index
   examples/index

Getting Started
---------------

Requirements
~~~~~~~~~~~~

- C99-compliant compiler
- CMake 3.14 or higher

Installation
~~~~~~~~~~~~

Using CMake:

.. code-block:: bash

   git clone https://github.com/your-username/libmyrtx.git
   cd libmyrtx
   mkdir build && cd build
   cmake ..
   cmake --build .
   cmake --install .

Using the build script:

.. code-block:: bash

   ./build.sh

Integration
~~~~~~~~~~~

To integrate libmyrtx into your CMake project:

.. code-block:: cmake

   add_subdirectory(path/to/libmyrtx)
   target_link_libraries(YourProject PRIVATE myrtx)

Indices and tables
==================

* :ref:`genindex`
* :ref:`search` 
