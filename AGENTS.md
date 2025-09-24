# Repository Guidelines

## Project Structure & Module Organization
- Source: `src/` (C99 implementations), headers in `include/myrtx/` (public API). Keep new headers under a logical subfolder (e.g., `include/myrtx/collections/`).
- Tests: `tests/` with one executable per unit (e.g., `avl_tree_test.c`).
- Examples: `examples/` minimal runnable demos.
- Docs: `docs/` (Sphinx + optional Doxygen). Build artifacts live under `docs/build/`.

## Build, Test, and Development Commands
- Configure & build: `mkdir -p build && cd build && cmake .. && cmake --build .`
- Toggle options: `cmake -DMYRTX_BUILD_EXAMPLES=OFF -DMYRTX_BUILD_TESTS=ON ..`
- Run tests: `cd build && ctest --output-on-failure`
- Run examples: `build/examples/arena_example` (paths relative to repo root after build).
- Helper script: `./build.sh --type Debug --test` (see `./build.sh --help`).
- Docs: `./build.sh --docs` or `cd docs && make html` (HTML under `docs/build/html`).

## Coding Style & Naming Conventions
- Language: C99. Debug builds use `-Wall -Wextra -Werror`; keep warnings at zero.
- Formatting: `.clang-format` (LLVM base, 4 spaces, width 100, no tabs). Apply with `clang-format -i` before commits.
- API prefixes: functions `myrtx_*`, types `myrtx_*_t`, macros/constants `MYRTX_*`.
- File names: snake_case (`hash_table.c`, `arena_allocator.h`). One module per pair in `src/<area>/` and `include/myrtx/<area>/`.

## Testing Guidelines
- Framework: CTest (via CMake). Each test is a small C program in `tests/` named `*_test.c` and registered with `add_test(NAME <name> COMMAND <exe>)`.
- Scope: add focused tests for new behavior and edge cases; keep them fast.
- Run locally: `ctest --output-on-failure` from `build/`.

## Commit & Pull Request Guidelines
- Commits: imperative mood, concise subject; prefer scoped types seen in history (e.g., `docs:`, `fix:`, `build:`). Include rationale in the body when non-trivial.
- PRs: clear description, linked issues, test plan (commands + expected output), and impact on API/docs. Add screenshots only for docs rendering changes.

## Agent-Specific Notes
- Keep diffs minimal and aligned with existing structure. When adding API, update headers, implementation, tests, and docs together.
- Do not break existing symbols without deprecation notes and migration hints in `docs/`.

## Local Checks & CI
- Pre-commit: install and enable hooks
  - `pip install pre-commit && pre-commit install`
  - Run on demand: `pre-commit run --all-files`
- CI runs on pushes/PRs and enforces `format-check` and `ctest` (see `.github/workflows/ci.yml`).

## Roadmap & Tasks
- See `TODO.md` for prioritized fixes, enhancements, and CI/doc tasks.
