# Expand CI matrix: macOS, Windows, Release, AddressSanitizer

Summary
- Extend GitHub Actions to build and test on Ubuntu, macOS, and Windows. Add Release build and an AddressSanitizer job (Linux).

Scope
- Update `.github/workflows/ci.yml` with a matrix for OS and build type, plus a dedicated ASan job.

Acceptance Criteria
- CI runs on all platforms; format-check and tests pass across matrix.
- ASan job executes and fails on memory errors.

References
- `.github/workflows/ci.yml`

Labels
- ci, priority: medium

