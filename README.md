# libmyrtx - MY Runtime eXtension

libmyrtx is a C99 library that provides useful functions and data structures for use in various C projects. The library focuses on memory allocators, strings, and other fundamental data structures.

[![CI](https://github.com/mgrossmann/libmyrtx/actions/workflows/ci.yml/badge.svg)](https://github.com/mgrossmann/libmyrtx/actions/workflows/ci.yml)
[![Documentation Status](https://readthedocs.org/projects/libmyrtx/badge/?version=latest)](https://libmyrtx.readthedocs.io/en/latest/?badge=latest)

## Features

Currently implemented features:

- **Arena Allocator**: A fast, region-based memory allocator with support for:
  - Allocation without explicit deallocation of individual objects
  - Temporary arenas with marker-based reset
  - Scratch arenas for local, temporary allocations
  - Aligned memory allocation

- **Context System**: Context management and error handling system:
  - Thread-local context stack
  - Extensible error handling
  - Support for user-defined extensions

- **String Functions**: Utility functions for string manipulation:
  - Duplication and formatting
  - Splitting and joining
  - Case conversion and trimming
  - Replacement and substring operations

- **String Type**: Dynamic string type for easier string handling:
  - Dynamic resizing and memory management
  - Safe string operations
  - Integration with the arena allocator

- **Hash Table**: High-performance hash table implementation:
  - Generic key-value storage
  - Linear probing for collision resolution
  - Support for custom hash and equality functions
  - Automatic resizing for performance

- **AVL Tree**: Self-balancing binary search tree:
  - O(log n) complexity for insertion, deletion, and search
  - In-order, pre-order, and post-order traversal
  - Min/max key retrieval
  - Support for custom comparison functions

## Requirements

- C99-compliant compiler
- CMake 3.14 or higher

## Quickstart

### Arena Allocator: Simplify Memory Management

```c
#include "myrtx/myrtx.h"

int main() {
    // Initialize arena
    myrtx_arena_t arena = {0};
    myrtx_arena_init(&arena, 0);
    
    // Allocate once, no need to free individual allocations
    char* str = myrtx_arena_alloc(&arena, 100);
    void* data = myrtx_arena_alloc(&arena, 1024);
    
    // Use temporary region and reset it
    size_t marker = myrtx_arena_temp_begin(&arena);
    void* temp = myrtx_arena_alloc(&arena, 512);
    // ... work with temporary memory ...
    myrtx_arena_temp_end(&arena, marker);  // Reset to marker
    
    // When done, free everything at once
    myrtx_arena_free(&arena);
    return 0;
}
```

### AVL Tree: Ordered Key-Value Storage

```c
#include "myrtx/myrtx.h"

int main() {
    // Initialize arena and tree
    myrtx_arena_t arena = {0};
    myrtx_arena_init(&arena, 0);
    myrtx_avl_tree_t* tree = myrtx_avl_tree_create(&arena, myrtx_avl_compare_strings, NULL);
    
    // Insert key-value pairs
    int value = 42;
    myrtx_avl_tree_insert(tree, "key", &value, NULL);
    
    // Find values
    void* found;
    if (myrtx_avl_tree_find(tree, "key", &found)) {
        printf("Found: %d\n", *(int*)found);
    }
    
    // Traversal in sorted order
    myrtx_avl_tree_traverse_inorder(tree, 
        (bool (*)(const void*, void*, void*))((bool(const void* k, void* v, void* ud) {
            printf("%s: %d\n", (char*)k, *(int*)v);
            return true;
        })), NULL);
    
    // Clean up everything at once
    myrtx_avl_tree_free(tree, NULL, NULL);
    myrtx_arena_free(&arena);
    return 0;
}
```

## Documentation

Comprehensive documentation is available at [libmyrtx.readthedocs.io](https://libmyrtx.readthedocs.io/).

The documentation includes:
- API reference for all components
- Detailed guides for each module
- Practical examples
- Integration and usage instructions

To build the documentation locally:

```bash
# Install documentation requirements
pip install -r docs/requirements.txt

# Build the documentation
cd docs
make html

# View the documentation (output in build/html/index.html)
```

## Integration

### Using with CMake

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/libmyrtx)
target_link_libraries(YourProject PRIVATE myrtx)
```

### Manual Integration

```bash
# Build and install the library
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/your/install/path
cmake --build . --target install

# Then link with -lmyrtx and include the headers
```

## Build Instructions

```bash
# Clone the repository
git clone https://github.com/your-username/libmyrtx.git
cd libmyrtx

# Create a build directory
mkdir build
cd build

# Configure and build with CMake
cmake ..
cmake --build .

# Run tests
ctest

# Run examples
./examples/arena_example
./examples/avl_tree_example
```

Alternatively, you can use the provided build script:

```bash
# Run default debug build
./build.sh

# Run release build with tests
./build.sh --type Release --test

# See all options
./build.sh --help
```

## Project Structure

```
libmyrtx/
├── include/                # Public header files
│   └── myrtx/
│       ├── myrtx.h         # Main header
│       ├── version.h       # Version information
│       ├── memory/
│       │   └── arena_allocator.h
│       ├── context/
│       │   └── context.h
│       ├── string/
│       │   └── string.h
│       └── collections/
│           ├── hash_table.h
│           └── avl_tree.h
├── src/                    # Implementations
│   ├── memory/
│   │   └── arena_allocator.c
│   ├── context/
│   │   └── context.c
│   ├── string/
│   │   └── string.c
│   └── collections/
│       ├── hash_table.c
│       └── avl_tree.c
├── examples/               # Example programs
│   ├── CMakeLists.txt
│   ├── arena_example.c
│   └── avl_tree_example.c
├── tests/                  # Tests
│   ├── CMakeLists.txt
│   ├── arena_test.c
│   └── avl_tree_test.c
├── docs/                   # Documentation
│   ├── source/             # Documentation source
│   ├── Makefile            # Documentation build script
│   └── requirements.txt    # Documentation dependencies
├── CMakeLists.txt          # Main CMake file
├── build.sh                # Build script
└── README.md               # This file
```

## License

This project is licensed under the MIT License. See `LICENSE` for more details.

## Contributing

Contributions are welcome! Please open an issue first to discuss changes before creating a pull request. See [TODO.md](TODO.md) for the current task list and roadmap.
