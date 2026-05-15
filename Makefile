CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=600 -I src/
BINARY   = mermaid-tui
SRCS     = src/main.cpp src/canvas/canvas.cpp src/parser/parser.cpp src/layout/layout.cpp src/render/render.cpp

# FULL=1 disables smart mode for exhaustive checks (e.g. clang-tidy)
FULL ?= 0

.DEFAULT_GOAL := help

.PHONY: all build clean setup install hooks \
	format format-code format-md format-yaml format-scripts \
	lint lint-code lint-format-code lint-cppcheck lint-md lint-yaml lint-makefile lint-scripts \
	tidy complexity comment-ratio docs \
	test test-unit coverage coverage-report \
	sast sast-security sast-secret \
	check quick-check full-check bump precommit prepush workflow help \
	major minor patch

##@ Workflow (daily flow)

workflow: ## Show the recommended workflow steps
	@echo ""
	@echo "  Daily workflow:"
	@echo "  ─────────────────────────────────────────────"
	@echo "  1. make build          Build the project"
	@echo "  2. make quick-check    Quick checks (<5s)"
	@echo "  3. make full-check     Full checks before push"
	@echo "  4. make test           Run tests"
	@echo ""

quick-check: format ## Quick quality check (<5s)
full-check: check ## Full quality check before push

##@ Getting Started

setup: ## Install all dependencies
	@bash scripts/dev/setup.sh

build: all ## Build the project

install: all ## Install to /usr/local/bin
	@cp $(BINARY) /usr/local/bin/$(BINARY)
	@echo "Installed to /usr/local/bin/$(BINARY)"

hooks: ## Install git hooks (pre-commit, pre-push)
	@cp scripts/git/pre-commit.sh .git/hooks/pre-commit
	@cp scripts/git/pre-push.sh .git/hooks/pre-push
	@chmod +x .git/hooks/pre-commit .git/hooks/pre-push
	@echo "Git hooks installed (pre-commit, pre-push)."

clean: ## Remove build artifacts
	rm -rf $(BINARY) build-cov/

##@ Aggregators

format: format-code format-md format-yaml format-scripts ## Auto-format all files

lint: lint-code lint-md lint-yaml lint-makefile lint-scripts tidy complexity comment-ratio docs ## Run all passive checks

test: test-unit ## Run all tests

check: lint test sast ## Full quality gate (CI/pre-push)

##@ Formatting

format-code: ## Format C++ code (clang-format)
	@bash lib/cpm/shell/run.sh format-code bash lib/cpm/checks/cpp/format-code.sh

format-md: ## Format Markdown files (rumdl)
	@bash lib/cpm/shell/run.sh format-md bash lib/cpm/checks/universal/format-md.sh

format-yaml: ## Format YAML files (trailing whitespace)
	@bash lib/cpm/shell/run.sh format-yaml bash lib/cpm/checks/universal/format-yaml.sh

format-scripts: ## Format shell scripts (shfmt)
	@bash lib/cpm/shell/run.sh format-scripts bash lib/cpm/checks/universal/format-scripts.sh

##@ Linting

lint-code: lint-format-code lint-cppcheck ## Lint C++ code (format + cppcheck)

lint-format-code: ## Check C++ formatting (no changes)
	@echo "==> checking C++ formatting..."
	@find src -name '*.cpp' -o -name '*.h' | xargs clang-format --dry-run -Werror --style=file:.config/.clang-format
	@echo "  [done] lint-format-code"

lint-cppcheck: ## Run cppcheck static analysis
	@bash lib/cpm/shell/run.sh lint-code bash lib/cpm/checks/cpp/lint-code.sh

lint-md: ## Lint Markdown files (rumdl)
	@bash lib/cpm/shell/run.sh lint-md bash lib/cpm/checks/universal/lint-md.sh

lint-yaml: ## Lint YAML files (yamllint)
	@bash lib/cpm/shell/run.sh lint-yaml bash lib/cpm/checks/universal/lint-yaml.sh

lint-makefile: ## Check Makefile conventions
	@bash lib/cpm/shell/run.sh check-makefile bash lib/cpm/checks/universal/check-makefile.sh

lint-scripts: ## Check shell script conventions (shellcheck)
	@bash lib/cpm/shell/run.sh check-scripts bash lib/cpm/checks/universal/check-scripts.sh

tidy: all ## Run clang-tidy (smart: changed files only)
	@bash lib/cpm/shell/run.sh run-tidy bash lib/cpm/checks/cpp/run-tidy.sh $(if $(filter 1,$(FULL)),--full)

complexity: all ## Check cyclomatic complexity (pmccabe)
	@bash lib/cpm/shell/run.sh check-complexity bash lib/cpm/checks/cpp/check-complexity.sh

comment-ratio: ## Show comment ratio per file
	@bash lib/cpm/shell/run.sh check-comment-ratio bash lib/cpm/checks/universal/check-comment-ratio.sh

docs: ## Check doxygen warnings
	@echo "==> checking doxygen..."
	@output=$$(doxygen .config/Doxyfile 2>&1) || { echo "doxygen failed"; exit 1; }; \
	echo "$$output" | grep "warning:" | grep -v "No output formats\|Unsupported xml" && exit 1 || true
	@echo "  [done] docs"

##@ Testing

test-unit: all ## Run unit tests
	@CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" SRCS="$(SRCS)" bash scripts/test/build-tests.sh
	@bash scripts/test/run-unit.sh

coverage: ## Build with coverage and run tests
	@bash scripts/test/run-coverage.sh

coverage-report: coverage ## Show coverage summary
	@bash scripts/test/report-coverage.sh

##@ Security

sast: sast-security sast-secret ## Run all SAST checks

sast-security: ## Run semgrep security scan
	@echo "==> running sast-security (semgrep)..."
	@if command -v semgrep >/dev/null; then \
		semgrep scan --config auto --error --quiet 2>&1 | grep -v "┌────\|Semgrep CLI\|└─────────────" || true; \
	else echo "  [skip] semgrep not installed"; fi
	@echo "  [done] sast-security"

sast-secret: ## Run gitleaks secret scan
	@bash lib/cpm/shell/run.sh sast-secret bash lib/cpm/checks/universal/sast-secret.sh

##@ Development

bump: ## Bump version (make bump PART=patch|minor|major)
	@bash scripts/dev/bump.sh "$(or $(PART),$(filter major minor patch,$(MAKECMDGOALS)))"

major minor patch:
	@true

precommit: ## Run pre-commit checks
	@bash scripts/git/precommit-check.sh

prepush: ## Run pre-push checks
	@bash scripts/git/prepush-check.sh

##@ Help

help: ## Show this help
	@awk 'BEGIN {FS = ":.*##"; printf "Usage:\n  make \033[36m<target>\033[0m\n"} \
		/^[a-zA-Z_0-9-]+:.*?##/ { printf "  \033[36m%-25s\033[0m %s\n", $$1, $$2 } \
		/^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) }' $(MAKEFILE_LIST)

# Internal targets
all: check-deps-quiet
	@$(CXX) $(CXXFLAGS) -o $(BINARY) $(SRCS) -lm

check-deps-quiet:
	@bash lib/cpm/checks/cpp/check-deps.sh

check-deps:
	@bash lib/cpm/checks/cpp/check-deps.sh --verbose

check-versions:
	@bash scripts/lint/check-versions.sh
