# TODO

## High Priority
- [ ] Arena temp markers: store a stable reference for the current block (pointer or index) rather than a byte offset; fix restore logic in `src/memory/arena_allocator.c`. Add tests that span multiple blocks (set marker → allocate → end → verify freed blocks/current pointer/stats).
  - Build/tests: `cmake -S . -B build && cmake --build build && ctest --output-on-failure`
- [ ] Context ownership: add `owns_global_arena` to `myrtx_context_t`; set in `myrtx_context_create` and free conditionally in `myrtx_context_destroy` (see `src/context/context.c`). Add tests for external vs. internal arena ownership.
- [ ] String replace capacity: after arena-path replacement in `myrtx_string_replace`, set `capacity = length + 1` (actual allocation), not `temp->capacity`. Add regression test.

## Medium Priority
- [ ] Align substring behavior: make `myrtx_string_substr` (new API) consistent with legacy `myrtx_substr` on out-of-range (return empty vs. NULL) and document the chosen behavior.
- [ ] Normalize comments in `src/collections/*` and `include/myrtx/collections/*` to English for consistency.
- [ ] CI matrix: add macOS and Windows jobs; include Release build and an AddressSanitizer job.
- [ ] Coverage: add gcov/llvm-cov target and optional CI artifact upload.
- [ ] CMake toggles for sanitizers: `MYRTX_ENABLE_ASAN`, `MYRTX_ENABLE_UBSAN` (Debug-only by default).
- [ ] Benchmarks: add simple benchmarks for arena, hash table, and AVL under `examples/` or `bench/`.
- [ ] Docs: document memory ownership semantics (context/global vs. temp arenas) and scratch pool usage; update guides and examples.

## Low Priority
- [ ] Pre-commit: optionally add `cmake-format` and `codespell` hooks.
- [ ] Packaging: generate CMake package config for consumers (`CMakePackageConfigHelpers`) to enable `find_package(myrtx)`.

## Notes
- Formatting check: `cmake --build build --target format-check`
- Run tests: `ctest --test-dir build --output-on-failure`
