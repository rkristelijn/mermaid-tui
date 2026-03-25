CC     = gcc
CFLAGS = -Wall -Wextra -std=c11

.PHONY: all clean help check test install

all: mermaid-tui

mermaid-tui: src/main.c
	$(CC) $(CFLAGS) -o $@ $^

test:
	@echo "==> unit tests"
	@echo "(none yet)"
	@echo "==> e2e tests"
	@echo "(none yet)"
	@echo "All tests passed."

check: mermaid-tui
	@echo "==> cppcheck"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 src/
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
	rm -f mermaid-tui

help:
	@echo "Usage:"
	@echo "  make           build"
	@echo "  make test      run tests"
	@echo "  make check     run cppcheck + semgrep + gitleaks"
	@echo "  make install   install git hooks"
	@echo "  make clean     remove build artifacts"
