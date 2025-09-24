# Fix arena temp marker encoding and restore logic

Summary
- Replace byte-offset encoding of the current block in `myrtx_arena_temp_begin`/`myrtx_arena_temp_end` with a stable reference (pointer or index). Current math assumes contiguous blocks and is undefined.

Scope
- Update marker representation and restore algorithm in `src/memory/arena_allocator.c`.
- Add tests that span multiple blocks and verify: freed blocks after `temp_end`, correct `current` block, and accurate stats.

Acceptance Criteria
- New tests pass and fail on current main before fix.
- No regressions in existing tests.
- Behavior documented in `docs/` (temporary regions, scratch arenas).

References
- `include/myrtx/memory/arena_allocator.h`
- `src/memory/arena_allocator.c`
- `tests/arena_test.c`

Labels
- bug, priority: high, area: memory
