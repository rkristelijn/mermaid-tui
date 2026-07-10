#!/usr/bin/env bash
# report-coverage.sh — Show coverage summary from gcov data.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

BUILD_DIR="${1:-build-cov}"

main() {
  echo "==> make coverage-report"
  if ! command -v lcov >/dev/null; then
    echo "  [skip] lcov not installed"
    exit 0
  fi

  lcov --capture --directory "${BUILD_DIR}" --output-file "${BUILD_DIR}/coverage.info" --quiet
  lcov --remove "${BUILD_DIR}/coverage.info" '/usr/*' --output-file "${BUILD_DIR}/coverage.info" --quiet
  lcov --list "${BUILD_DIR}/coverage.info"
  echo "  [done] coverage-report"
}

main "$@"
