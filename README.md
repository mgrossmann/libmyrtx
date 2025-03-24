# libmyrtx - MY Runtime eXtension

libmyrtx is a C99 library that provides useful functions and data structures for use in various C projects. The library focuses on memory allocators, strings, and other fundamental data structures.

## Features

Currently implemented features:

- **Arena Allocator**: A fast, region-based memory allocator with support for:
  - Allocation without explicit deallocation of individual objects
  - Temporary arenas with marker-based reset
  - Scratch arenas for local, temporary allocations
  - Aligned memory allocation

## Requirements

- C99-compliant compiler
- CMake 3.14 or higher

## Usage

### Integration into a CMake project

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/libmyrtx)
target_link_libraries(YourProject PRIVATE myrtx)
```

### Example of using the Arena Allocator

```c
#include "myrtx/myrtx.h"
#include <stdio.h>

int main(void) {
    // Initialize arena
    myrtx_arena_t arena = {0};
    myrtx_arena_init(&arena, 0); // 0 uses the default block size
    
    // Allocate memory from the arena
    void* memory = myrtx_arena_alloc(&arena, 1024);
    
    // Allocate aligned memory
    void* aligned_memory = myrtx_arena_alloc_aligned(&arena, 2048, 64);
    
    // Allocate memory initialized with zeros
    void* zeroed_memory = myrtx_arena_calloc(&arena, 512);
    
    // Use a temporary arena
    size_t marker = myrtx_arena_temp_begin(&arena);
    void* temp_memory = myrtx_arena_alloc(&arena, 4096);
    // ... temporary operations ...
    myrtx_arena_temp_end(&arena, marker); // Resets to the marker position
    
    // Use a scratch arena (region with automatic deallocation)
    {
        myrtx_scratch_arena_t scratch = {0};
        myrtx_scratch_begin(&scratch, &arena);
        
        void* scratch_memory = myrtx_arena_alloc(scratch.arena, 8192);
        // ... temporary operations ...
        
        myrtx_scratch_end(&scratch); // Automatic deallocation when leaving the scope
    }
    
    // Reset the entire arena
    myrtx_arena_reset(&arena);
    
    // Free the arena when no longer needed
    myrtx_arena_free(&arena);
    
    return 0;
}
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
├── include/              # Public header files
│   └── myrtx/
│       ├── myrtx.h       # Main header
│       ├── version.h     # Version information
│       └── memory/
│           └── arena_allocator.h
├── src/                  # Implementations
│   └── memory/
│       └── arena_allocator.c
├── examples/             # Example programs
│   ├── CMakeLists.txt
│   └── arena_example.c
├── tests/                # Tests
│   ├── CMakeLists.txt
│   └── arena_test.c
├── CMakeLists.txt        # Main CMake file
├── build.sh              # Build script
└── README.md             # This file
```

## License

This project is licensed under the MIT License. See `LICENSE` for more details.

## Contributing

Contributions are welcome! Please open an issue first to discuss changes before creating a pull request. 