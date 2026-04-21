#!/usr/bin/env bash
# run-coverage.sh — Build with coverage flags and run all tests.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

BUILD_DIR="${1:-build-cov}"

main() {
  echo "==> make coverage (building with --coverage...)"
  mkdir -p "${BUILD_DIR}"
  g++ -std=c++17 --coverage -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=600 \
    -o "${BUILD_DIR}/mermaid-tui" src/main.cpp src/canvas.cpp -lm

  echo "==> make coverage (running tests...)"
  local found=0
  for t in "${BUILD_DIR}"/test_*; do
    [ -x "$t" ] || continue
    "$t"
    (( found++ )) || true
  done
  if (( found == 0 )); then
    echo "  [skip] no test binaries found in ${BUILD_DIR}/"
  fi
  echo "  [done] coverage"
}

main "$@"
