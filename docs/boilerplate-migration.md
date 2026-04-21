# Boilerplate migration: llama-cli → mermaid-tui

Goal: same quality gate, pre-commit/pre-push hooks, CI pipeline and developer
experience as `llama-cli`, adapted for a C project (gcc, no CMake/doctest).

## Current state mermaid-tui

- Makefile: basic (build, test placeholder, monolithic `check` target)
- hooks/pre-commit: simple (block main, build check)
- .github/workflows/ci.yml: 5 separate jobs, no scripts, no concurrency
- .clang-tidy: present, good
- Missing: `.editorconfig`, `.config/`, `scripts/`, pre-push hook, formatting,
  lint-scripts, lint-makefile, lint-yaml, lint-md, coverage, comment-ratio,
  complexity, versions.env, install-deps.sh, setup.sh

## Phasing

Each phase is a self-contained PR that must be green before the next starts.
Steps within a phase are in order.

---

### Phase 1 — Foundation (config + directory structure)

No behavior change, just lay down files.

1. **`.editorconfig`** — copy 1:1 from llama-cli
2. **`.config/` directory**:
   - `.config/.clang-format` — copy from llama-cli (Google-based, 140 col)
   - `.config/.clang-tidy` — move current `.clang-tidy` into `.config/`,
     adapt for C (no `CamelCase` classes, `lower_case` everything)
   - `.config/rumdl.toml` — copy, adjust excludes (no cmake `build/` dir)
   - `.config/yamllint.yml` — copy 1:1
   - `.config/Doxyfile` — create for mermaid-tui (C, not C++)
   - `.config/versions.env` — copy, remove C++-specific entries
     (cmake, ccache, doctest), add gcc
3. **`scripts/` directory structure** (empty dirs):
   - `scripts/fmt/`, `scripts/lint/`, `scripts/test/`, `scripts/git/`,
     `scripts/ci/`, `scripts/dev/`
4. **`.gitignore`** extend: `build/`, `build-cov/`, `.cache/`,
   `*.gcda`, `*.gcno`, `*.log`, `.env`, `coverage.info`
5. **`VERSION`** — keep as-is (already present)

Verification: `git status` shows only new/moved files, build still works
with `make`.

---

### Phase 2 — Scripts (fmt + lint + test)

All scripts follow llama-cli convention: `#!/usr/bin/env bash`, safety flags,
header comment, kebab-case.

**Formatting scripts:**

1. `scripts/fmt/format-code.sh` — `find src -name '*.c' -o -name '*.h' | xargs clang-format`
   with `--style=file:.config/.clang-format`
2. `scripts/fmt/format-md.sh` — copy 1:1
3. `scripts/fmt/format-yaml.sh` — copy 1:1
4. `scripts/fmt/format-scripts.sh` — copy, adjust `find` paths

**Lint scripts:**

5. `scripts/lint/lint-code.sh` — cppcheck for C (adjust suppressions)
6. `scripts/lint/lint-md.sh` — copy 1:1
7. `scripts/lint/lint-yaml.sh` — copy 1:1
8. `scripts/lint/check-scripts.sh` — copy 1:1
9. `scripts/lint/check-makefile.sh` — copy 1:1
10. `scripts/lint/run-tidy.sh` — adapt: `-std=c11` instead of `-std=c++17`,
    `*.c` instead of `*.cpp`, no smart mode (no CMake compile_commands.json)
11. `scripts/lint/check-complexity.sh` — copy, adjust `find` for `*.c`
12. `scripts/lint/check-comment-ratio.sh` — copy 1:1
13. `scripts/lint/check-deps.sh` — adjust REQUIRED/OPTIONAL for C toolchain
14. `scripts/lint/check-versions.sh` — copy, adjust for versions.env

**Test scripts:**

15. `scripts/test/run-unit.sh` — adapt: find `test/test_*` binaries instead of
    cmake build dir
16. `scripts/test/run-coverage.sh` — gcc `--coverage` flags, no cmake
17. `scripts/test/report-coverage.sh` — lcov on gcc coverage data

**CI scripts:**

18. `scripts/ci/install-deps.sh` — copy, remove cmake/ccache, adapt for C

**Dev scripts:**

19. `scripts/dev/setup.sh` — copy, remove C++-specific tools, adapt
20. `scripts/dev/bump.sh` — copy 1:1 (operates on VERSION file)

Verification: each script runs standalone without errors.

---

### Phase 3 — Makefile restructuring

Replace the current monolithic Makefile with the llama-cli structure.

1. **Makefile targets** (same layout as llama-cli):
   - `##@ Getting Started`: `setup`, `build`/`all`, `install`, `hooks`, `clean`
   - `##@ Aggregators`: `format`, `lint`, `test`, `check`, `full-check`
   - `##@ Formatting`: `format-code`, `format-md`, `format-yaml`, `format-scripts`
   - `##@ Linting`: `lint-code` (format-check + cppcheck), `lint-md`, `lint-yaml`,
     `lint-makefile`, `lint-scripts`, `tidy`, `complexity`, `comment-ratio`, `docs`
   - `##@ Testing`: `test-unit`, `coverage`, `coverage-report`
   - `##@ Security`: `sast`, `sast-security`, `sast-secret`
   - `##@ Development`: `bump`, `precommit`, `prepush`
   - `##@ Help`: `help` (awk-based, self-documenting)
2. **`lint-format-code`** target: `clang-format --dry-run -Werror` with
   `--style=file:.config/.clang-format`
3. **`check-deps`** target: calls `scripts/lint/check-deps.sh`
4. Build target: `gcc` direct (no cmake), output `mermaid-tui`

Verification: `make help` shows all targets, `make build` compiles,
`make check` runs all checks.

---

### Phase 4 — Git hooks (pre-commit + pre-push)

1. **`scripts/git/pre-commit.sh`** — copy from llama-cli, adapt:
   - Block main
   - Auto-format staged `*.c` and `*.h` files
   - Call `scripts/git/precommit-check.sh`
2. **`scripts/git/pre-push.sh`** — copy 1:1 (delegates to prepush-check)
3. **`scripts/git/precommit-check.sh`** — copy from llama-cli:
   - format-code, format-yaml, format-md, format-scripts, sast-secret
   - No `index` step (no INDEX.md in this project)
4. **`scripts/git/prepush-check.sh`** — copy from llama-cli, adjust STEPS:
   - Lint: lint-code, lint-yaml, lint-md, lint-makefile, lint-scripts
   - Analysis: tidy, complexity
   - Build: build
   - Test: test-unit
   - Security: sast-security, sast-secret
   - Metrics: comment-ratio
5. **`make hooks`** target: copy pre-commit.sh and pre-push.sh to `.git/hooks/`
6. Remove `hooks/` directory (scripts now live in `scripts/git/`)

Verification: `make hooks && git commit --allow-empty` triggers pre-commit,
`make prepush` runs all checks.

---

### Phase 5 — CI pipeline

Replace current ci.yml with the llama-cli structure.

1. **Concurrency**: `group: ci-${{ ref }}`, `cancel-in-progress: true`
2. **Changes job** with `dorny/paths-filter` (skip code-only checks on
   docs-only changes)
3. **Pin actions** on SHA (not `@v4`)
4. **Jobs** (parallel where possible):
   - `version-bump` — check VERSION on PRs to main
   - `lint-cpp` — `make lint-format-code` (code-only)
   - `lint-yaml` — `make lint-yaml`
   - `lint-markdown` — `make lint-md`
   - `lint-makefile` — `make lint-makefile`
   - `lint-scripts` — `make lint-scripts`
   - `lint-tidy` — `make tidy` (code-only)
   - `lint-cppcheck` — `make lint-cppcheck` (code-only)
   - `lint-docs` — `make docs` (code-only)
   - `lint-complexity` — `make complexity` (code-only)
   - `build` — `make build` (code-only)
   - `unit-test` — `make test-unit` (code-only)
   - `test-coverage` — lcov + codecov (code-only)
   - `sast-security` — semgrep container
   - `sast-secret` — gitleaks
   - `comment-ratio` — `make comment-ratio` (code-only)
5. **`ubuntu-24.04`** instead of `ubuntu-latest`
6. **`install-deps.sh`** per job (only what's needed)

Verification: push to feature branch, all jobs green.

---

### Phase 6 — Cleanup + documentation

1. Remove `hooks/` directory (replaced by `scripts/git/`)
2. Remove `.clang-tidy` from root (moved to `.config/`)
3. Remove `Doxyfile` from root (moved to `.config/`)
4. Update `CONTRIBUTING.md`:
   - `make setup` as first step
   - `make check` as quality gate
   - `make hooks` for git hooks
   - Point to `make help` for all targets
5. Update `docs/boilerplate.md` — mark as superseded by llama-cli structure
6. Update `README.md` — add `make setup` and `make check`

Verification: `make check` green, `make help` complete, docs accurate.

---

## Out of scope (not relevant for C project)

- CMakeLists.txt / cmake build system (using gcc directly)
- doctest / C++ test framework (using C assert-based tests)
- `scripts/gh/` (GitHub PR/issue tooling — maybe later)
- `scripts/dev/build-index.sh` (INDEX.md — not in this project)
- `scripts/dev/todo.sh`, `scripts/dev/log-viewer.sh` (llama-cli specific)
- `.kiro/` agent config (project-specific)
- `e2e/` directory (later, when there's a binary worth e2e testing)
- `install.sh` (later, when there's a release)
- `.config/cmake/`, `.config/cliff.toml` (no cmake, no changelog tooling yet)

## Summary

```text
Phase 1: config + dirs        → foundation, no behavior change
Phase 2: scripts              → all checks as standalone scripts
Phase 3: Makefile             → everything callable via make
Phase 4: git hooks            → pre-commit + pre-push
Phase 5: CI pipeline          → GitHub Actions
Phase 6: cleanup + docs       → clean state
```

Each phase is its own PR. Phases 1-3 can go fast. Phases 4-5 are the real
value. Phase 6 is polish.
