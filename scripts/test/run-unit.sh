#!/usr/bin/env bash
# run-unit.sh — Build and run all unit tests.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

BUILD_DIR="${1:-build}"

main() {
  echo "==> make test-unit"
  local found=0
  for t in "${BUILD_DIR}"/test_*; do
    [ -x "$t" ] || continue
    echo "  [running] $(basename "$t")"
    "$t"
    (( found++ )) || true
  done
  if (( found == 0 )); then
    echo "  [skip] no test binaries found in ${BUILD_DIR}/"
  fi
  echo "  [done] test-unit"
}

main "$@"
