# Add sanitizer toggles to CMake (ASan/UBSan)

Summary
- Introduce `MYRTX_ENABLE_ASAN` and `MYRTX_ENABLE_UBSAN` CMake options (Debug default) for easier local and CI debugging.

Scope
- Update top-level `CMakeLists.txt` to append sanitizer flags when options are ON.
- Document usage in README/AGENTS.

Acceptance Criteria
- Flags apply only to supported compilers; builds succeed when disabled on unsupported.
- CI runs an ASan job without flakiness.

References
- `CMakeLists.txt`
- `.github/workflows/ci.yml`

Labels
- build, enhancement, priority: medium

