#!/usr/bin/env bash
# check-deps.sh — Verify required and optional tools are installed.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

VERBOSE=false
[[ "${1-}" == "--verbose" ]] && VERBOSE=true

REQUIRED=(g++ clang-format cppcheck cloc doxygen)
OPTIONAL=(clang-tidy pmccabe semgrep gitleaks shellcheck yamllint rumdl)

missing=0
warnings=0

find_tool() {
  local tool="$1"
  if command -v "${tool}" >/dev/null 2>&1; then
    return 0
  fi
  if [[ "${tool}" == "clang-tidy" && -f "/opt/homebrew/opt/llvm/bin/clang-tidy" ]]; then
    return 0
  fi
  return 1
}

main() {
  if [[ "${VERBOSE}" == true ]]; then
    echo "==> checking required tools..."
    for tool in "${REQUIRED[@]}"; do
      if find_tool "${tool}"; then
        printf "  %-20s ✓\n" "${tool}"
      else
        printf "  %-20s MISSING\n" "${tool}"
        (( missing++ )) || true
      fi
    done

    echo ""
    echo "==> checking optional tools..."
    for tool in "${OPTIONAL[@]}"; do
      if find_tool "${tool}"; then
        printf "  %-20s ✓\n" "${tool}"
      else
        printf "  %-20s not installed (optional)\n" "${tool}"
        (( warnings++ )) || true
      fi
    done
    echo ""
  else
    for tool in "${REQUIRED[@]}"; do
      if ! find_tool "${tool}"; then
        (( missing++ )) || true
      fi
    done
  fi

  if (( missing > 0 )); then
    echo "${missing} required tool(s) missing. Run 'make setup' to install."
    exit 1
  fi
  if [[ "${VERBOSE}" == true ]]; then
    if (( warnings > 0 )); then
      echo "All required tools present. ${warnings} optional tool(s) missing."
    else
      echo "All tools present."
    fi
  fi
}

main "$@"
