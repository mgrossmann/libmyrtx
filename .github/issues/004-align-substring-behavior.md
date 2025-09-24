# Align myrtx_string_substr behavior with legacy API or document difference

Summary
- Decide and implement consistent behavior when `start >= length`: return empty string (like `myrtx_substr`) or NULL; document clearly.

Scope
- Update `myrtx_string_substr` and docs/guides.
- Add tests for out-of-range behavior and empty-length requests.

Acceptance Criteria
- Behavior is consistent and documented; tests cover edge cases.

References
- `include/myrtx/string/string.h`
- `src/string/string.c`
- `tests/string_test.c`

Labels
- enhancement, priority: medium, area: string

