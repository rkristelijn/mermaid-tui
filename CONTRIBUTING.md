# Contributing

## Getting started

```bash
git clone git@github.com:rkristelijn/mermaid-tui.git
cd mermaid-tui
make install   # install git hooks
make           # build
./mermaid-tui  # run
```

Requirements: `gcc`, `make`, `clang-tidy`, `cppcheck`, `pmccabe`, `doxygen`, `gitleaks`, `semgrep` (via pipx).

```bash
sudo apt install gcc make clang-tidy cppcheck pmccabe doxygen gitleaks
pipx install semgrep
```

## Build

```bash
make        # build binary
make clean  # remove binary
```

## Test

```bash
make test   # run unit + e2e tests
```

## Quality checks

```bash
make check  # run all checks
make ci     # build + test + check (full pipeline, same as GitHub Actions)
```

Checks in order:

1. **clang-tidy** — braces, naming, cognitive complexity, bugprone patterns
2. **pmccabe** — cyclomatic complexity ≤ 10 per function
3. **cppcheck** — static analysis
4. **doxygen** — undocumented public API
5. **semgrep** — security scan
6. **gitleaks** — secret scanning (full git history)

## Workflow

```text
main (protected — no direct commits)
  └── feat/<name>
        └── PR → CI → merge
```

1. Create a branch: `git checkout -b feat/my-change`
2. Make changes
3. Run `make ci` locally before pushing
4. Open a PR — CI runs automatically
5. Bump `VERSION` before merging to main (CI enforces this)

## Why so many checks?

This project is built with AI assistance. AI generates plausible-looking code that can be:

- Subtly wrong (logic errors that pass tests)
- Unnecessarily complex (accidental complexity)
- Poorly documented (hard to maintain)
- Insecure (leaked secrets, unsafe patterns)

The checks enforce a baseline that keeps the code maintainable regardless of who (or what) wrote it:

| Check | Prevents |
|-------|---------|
| clang-tidy | Sloppy style, hidden bugs, complex functions |
| pmccabe | Functions that are too hard to reason about |
| cppcheck | Memory errors, unused code, style issues |
| doxygen | Undocumented public API |
| semgrep | Security anti-patterns |
| gitleaks | Accidentally committed secrets |
| 20% comment ratio | Code with no explanation of intent |

## Code standards

- See [docs/commenting-standard.md](docs/commenting-standard.md) — Doxygen style
- Max function complexity: 10 (pmccabe + clang-tidy)
- Max function parameters: 5
- Max function lines: 40
- Comment ratio: ≥ 20% (`cloc src/ | grep comment`)
