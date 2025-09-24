# Respect global_arena ownership in context destroy

Summary
- Add an ownership flag (e.g., `owns_global_arena`) to `myrtx_context_t`. Only free `global_arena` if created internally. Avoid double-free when user supplies an external arena.

Scope
- Modify `include/myrtx/context/context.h` and `src/context/context.c` to add and honor ownership.
- Add tests covering both external and internally-created arenas.

Acceptance Criteria
- Tests demonstrate no double-free and correct teardown in both modes.
- No regressions in existing tests.

References
- `include/myrtx/context/context.h`
- `src/context/context.c`
- `tests/context_test.c`

Labels
- bug, priority: high, area: context

