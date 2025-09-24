# Provide CMake package config for find_package(myrtx)

Summary
- Generate and install CMake package config files (`myrtxConfig.cmake`, `myrtxConfigVersion.cmake`) so consumers can `find_package(myrtx)`.

Scope
- Use `CMakePackageConfigHelpers` in `CMakeLists.txt` and install to `lib/cmake/myrtx`.
- Document usage in README.

Acceptance Criteria
- `find_package(myrtx)` works in a sample consumer project after `cmake --install`.
- README includes snippet showing usage.

References
- `CMakeLists.txt`

Labels
- enhancement, priority: low

