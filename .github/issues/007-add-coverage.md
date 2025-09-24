# Add coverage target and CI artifact

Summary
- Add coverage generation (gcov/lcov or llvm-cov) and upload HTML artifacts in CI for visibility.

Scope
- Add CMake options/targets to build with coverage and generate reports.
- Extend CI to run coverage and upload artifact.

Acceptance Criteria
- Local: `cmake -DMYRTX_COVERAGE=ON` produces a coverage report.
- CI: artifact available for download; no test regressions.

References
- `CMakeLists.txt`
- `.github/workflows/ci.yml`

Labels
- ci, enhancement, priority: medium

