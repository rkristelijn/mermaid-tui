# Boilerplate

Reference setup for C projects in this workspace. Based on the `tasks` repo.

## File structure

```text
project/
├── src/
│   └── main.c
├── test/
│   ├── test_ids.c        # unit tests
│   ├── test_e2e.sh       # e2e tests against binary
│   └── fixtures/         # realistic test data
├── hooks/
│   └── pre-commit
├── docs/
├── .github/
│   └── workflows/
│       └── ci.yml
├── .gitignore
├── Makefile
├── VERSION
└── README.md
```

---

## Makefile

```makefile
CC     = gcc
CFLAGS = -Wall -Wextra -std=c11

.PHONY: all clean help check test install

all: <binary>

<binary>: src/main.c
	$(CC) $(CFLAGS) -o $@ $^

test:
	$(CC) $(CFLAGS) -o test/test_<module> src/<module>.c test/test_<module>.c && ./test/test_<module>

check: <binary>
	@echo "==> cppcheck"
	cppcheck --enable=all --suppress=missingIncludeSystem --suppress=unusedFunction --error-exitcode=1 src/
	@echo "==> semgrep"
	PATH="$$HOME/.local/bin:$$PATH" semgrep scan --config auto --error
	@echo "==> gitleaks"
	gitleaks detect --source .
	@echo "All checks passed."

install:
	cp hooks/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
	@echo "Git hooks installed."

clean:
	rm -f <binary> test/test_*

help:
	@echo "Usage:"
	@echo "  make           build"
	@echo "  make test      run unit + e2e tests"
	@echo "  make check     run cppcheck + semgrep + gitleaks"
	@echo "  make install   install git hooks"
	@echo "  make clean     remove build artifacts"
```

---

## .gitignore

```text
<binary>
*.o
test/test_*
```

---

## hooks/pre-commit

```sh
#!/bin/sh

# Block direct commits to main
branch=$(git symbolic-ref --short HEAD 2>/dev/null)
if [ "$branch" = "main" ]; then
    echo "ERROR: direct commits to main are not allowed. Use a feature branch."
    exit 1
fi

# Build if outdated
if [ ! -f <binary> ] || [ src/main.c -nt <binary> ]; then
    make -s || { echo "ERROR: build failed"; exit 1; }
fi

exit 0
```

Install with `make install`.

---

## GitHub Actions CI (`ci.yml`)

Five jobs, all run on push to `main` and on PRs:

| Job | Tool | Purpose |
|-----|------|---------|
| `version-bump` | git diff | Enforce VERSION bump on every PR to main |
| `build` | gcc | Compile with `-Wall -Wextra` |
| `test` | make test | Unit tests + e2e tests |
| `cppcheck` | cppcheck | Static analysis |
| `semgrep` | semgrep/semgrep image | SAST security scan |
| `gitleaks` | gitleaks-action@v2 | Secret scanning (full history) |

Key settings:

- `fetch-depth: 0` on gitleaks — scans full git history, not just latest commit
- `permissions: contents: read, pull-requests: read` — minimum needed for gitleaks-action
- semgrep runs in official Docker image — no install step needed
- `--config auto` on semgrep — auto-detects language and applies recommended rules

---

## Unit test pattern (C, no framework)

```c
#include <stdio.h>
#include "../src/<module>.h"

#define PASS "\033[32mPASS\033[0m"
#define FAIL "\033[31mFAIL\033[0m"

static int failures = 0;

#define assert_eq(desc, expected, actual) do { \
    if ((expected) == (actual)) { printf(PASS " %s\n", desc); } \
    else { printf(FAIL " %s: expected %d, got %d\n", desc, expected, actual); failures++; } \
} while(0)

int main(void) {
    assert_eq("description", expected_value, actual_value);

    printf("\n%s\n", failures == 0 ? "All tests passed." : "Some tests FAILED.");
    return failures > 0 ? 1 : 0;
}
```

---

## VERSION file

Plain semver, one line:

```text
0.1.0
```

Bump before every PR to main. CI enforces this via the `version-bump` job.

---

## Workflow

```text
main (protected)
  └── feat/<name>     ← work here
        └── PR → CI → merge
```

Pre-commit hook blocks direct commits to `main`. Always branch, always PR.
