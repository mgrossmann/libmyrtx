#!/usr/bin/env bash
set -euo pipefail

# Requires GitHub CLI: https://cli.github.com/
# Usage:
#   gh auth login
#   ./scripts/create_issues.sh

require_gh() {
  if ! command -v gh >/dev/null 2>&1; then
    echo "error: GitHub CLI 'gh' not found. Install from https://cli.github.com/" >&2
    exit 1
  fi
}

require_gh

# Create issues from prepared bodies. Labels are optional; remove if they don't exist.

gh issue create \
  --title "Fix arena temp marker encoding and restore logic" \
  --body-file .github/issues/001-arena-temp-markers.md \
  --label bug --label "priority: high" --label "area: memory"

gh issue create \
  --title "Respect global_arena ownership in context destroy" \
  --body-file .github/issues/002-context-ownership.md \
  --label bug --label "priority: high" --label "area: context"

gh issue create \
  --title "Correct capacity after arena-path replacement in myrtx_string_replace" \
  --body-file .github/issues/003-string-replace-capacity.md \
  --label bug --label "priority: high" --label "area: string"

gh issue create \
  --title "Align myrtx_string_substr behavior with legacy or document difference" \
  --body-file .github/issues/004-align-substring-behavior.md \
  --label enhancement --label "priority: medium" --label "area: string"

gh issue create \
  --title "Normalize comments in collections modules to English" \
  --body-file .github/issues/005-normalize-collections-comments.md \
  --label docs --label "priority: medium" --label "area: collections"

gh issue create \
  --title "Expand CI matrix: macOS, Windows, Release, ASan" \
  --body-file .github/issues/006-expand-ci-matrix.md \
  --label ci --label "priority: medium"

gh issue create \
  --title "Add coverage target and CI artifact" \
  --body-file .github/issues/007-add-coverage.md \
  --label ci --label enhancement --label "priority: medium"

gh issue create \
  --title "Add sanitizer toggles to CMake (ASan/UBSan)" \
  --body-file .github/issues/008-sanitizer-toggles-cmake.md \
  --label build --label enhancement --label "priority: medium"

gh issue create \
  --title "Add micro-benchmarks for arena, hash table, and AVL" \
  --body-file .github/issues/009-benchmarks.md \
  --label enhancement --label "priority: medium"

gh issue create \
  --title "Document ownership semantics and scratch pool usage" \
  --body-file .github/issues/010-docs-ownership-scratch-pool.md \
  --label docs --label "priority: medium"

gh issue create \
  --title "Add optional pre-commit hooks: cmake-format and codespell" \
  --body-file .github/issues/011-precommit-extras.md \
  --label devx --label "priority: low"

gh issue create \
  --title "Provide CMake package config for find_package(myrtx)" \
  --body-file .github/issues/012-cmake-package-config.md \
  --label enhancement --label "priority: low"

echo "Created issue drafts via gh. If labels are missing, run with existing labels or create them first."

