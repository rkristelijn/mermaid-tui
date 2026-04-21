#!/usr/bin/env bash
# check-makefile.sh — Check that Makefile targets don't have too much inline shell.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

MAX_LINES=5
MAKEFILE="Makefile"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --max) MAX_LINES="$2"; shift 2 ;;
    *)     MAKEFILE="$1"; shift ;;
  esac
done

main() {
  local target=""
  local count=0
  local failures=0

  while IFS= read -r line; do
    if [[ "${line}" =~ ^[a-zA-Z_][a-zA-Z0-9_-]*:.* ]] && [[ ! "${line}" =~ ^[A-Z_]+[[:space:]]*[\?:]?= ]]; then
      if [[ -n "${target}" ]] && (( count > MAX_LINES )); then
        printf "  %-25s %d lines (max %d)\n" "${target}" "${count}" "${MAX_LINES}"
        (( failures++ )) || true
      fi
      target="${line%%:*}"
      count=0
      continue
    fi

    if [[ "${line}" =~ ^$'\t' ]] && [[ -n "${target}" ]]; then
      local stripped="${line#$'\t'}"
      stripped="${stripped#@}"
      [[ "${stripped}" =~ ^echo[[:space:]] ]] && continue
      [[ "${stripped}" =~ ^\$\(MAKE\) ]] && continue
      (( count++ )) || true
    elif [[ -z "${line}" ]]; then
      :
    else
      if [[ -n "${target}" ]] && (( count > MAX_LINES )); then
        printf "  %-25s %d lines (max %d)\n" "${target}" "${count}" "${MAX_LINES}"
        (( failures++ )) || true
      fi
      target=""
      count=0
    fi
  done < "${MAKEFILE}"

  if [[ -n "${target}" ]] && (( count > MAX_LINES )); then
    printf "  %-25s %d lines (max %d)\n" "${target}" "${count}" "${MAX_LINES}"
    (( failures++ )) || true
  fi

  if (( failures > 0 )); then
    echo ""
    echo "  ${failures} target(s) exceed ${MAX_LINES}-line limit. Extract to scripts/."
    return 1
  fi
}

main "$@"
