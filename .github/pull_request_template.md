## Summary

Describe the change and the problem it solves. Mention any API additions or behavior changes.

## Type of change

- [ ] Feature
- [ ] Fix
- [ ] Docs
- [ ] Build/CI
- [ ] Refactor
- [ ] Tests
- [ ] Chore

## Details

- Motivation / Context:
- Notable design decisions:
- Backwards compatibility impact:

## Test Plan

Commands run locally (paste output or summarize):

```bash
mkdir -p build && cd build
cmake .. && cmake --build .
ctest --output-on-failure
```

## Checklist

- [ ] `cmake --build . --target format-check` passes
- [ ] All tests pass locally (`ctest`)
- [ ] Public headers updated as needed (`include/myrtx/`)
- [ ] Docs updated if behavior/API changed (`docs/`)
- [ ] Examples adjusted if applicable (`examples/`)

## Screenshots / Docs Preview (if UI/docs)

Optional: attach screenshots or note `docs/build/html/index.html` for a local preview.

