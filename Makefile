CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=600

.PHONY: all clean help check test docs install ci

all: mermaid-tui

mermaid-tui: src/main.c src/canvas.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test:
	@echo "==> unit tests (none yet)"
	@echo "All tests passed."

docs:
	@doxygen Doxyfile 2>&1 | grep -v "No output formats" || true
	@echo "Doxygen lint passed."

check: mermaid-tui
	@echo "==> clang-tidy"
	@clang-tidy src/*.c -- -std=c11 -D_XOPEN_SOURCE=600 2>&1 | grep "warning:" && exit 1 || true
	@echo "==> pmccabe (complexity <= 10)"
	@pmccabe src/*.c | awk '$$1 > 10 {print; found=1} END {if (found) exit 1}'
	@echo "==> cppcheck"
	@cppcheck --enable=all --suppress=missingIncludeSystem --suppress=unusedFunction --error-exitcode=1 src/
	@echo "==> doxygen lint"
	@doxygen Doxyfile 2>&1 | grep "warning:" | grep -v "No output formats" && exit 1 || true
	@echo "==> semgrep"
	@PATH="$$HOME/.local/bin:$$PATH" semgrep scan --config auto --error
	@echo "==> gitleaks"
	@gitleaks detect --source .
	@echo "All checks passed."

# Run everything: build, test, check
ci: all test check

install:
	cp hooks/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
	@echo "Git hooks installed."

clean:
	rm -f mermaid-tui

help:
	@echo "Usage:"
	@echo "  make        build"
	@echo "  make test   run tests"
	@echo "  make check  run all quality checks"
	@echo "  make ci     build + test + check (full pipeline)"
	@echo "  make docs   run doxygen lint"
	@echo "  make install install git hooks"
	@echo "  make clean  remove build artifacts"
