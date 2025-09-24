# Correct capacity after arena-path replacement in myrtx_string_replace

Summary
- When replacing via arena allocations, set `capacity` to `length + 1` (actual allocation) instead of inheriting `temp->capacity` to reflect real buffer size.

Scope
- Adjust `myrtx_string_replace` in `src/string/string.c`.
- Add regression test ensuring capacity matches data size and operations after replace succeed without overflow.

Acceptance Criteria
- New test passes and fails on current main before fix.
- No regressions in existing tests.

References
- `src/string/string.c`
- `tests/string_test.c`

Labels
- bug, priority: high, area: string

