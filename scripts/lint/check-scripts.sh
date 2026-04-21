#!/usr/bin/env bash
# check-scripts.sh — Validate shell script conventions.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

failures=0

fail() {
  echo "  FAIL: $1"
  (( failures++ )) || true
}

main() {
  echo "==> checking script conventions..."

  while IFS= read -r script; do
    local name dir line1
    name="$(basename "${script}")"
    dir="$(dirname "${script}")"

    if [[ "${dir}" == "scripts" ]]; then
      fail "${script}: must be in a subdirectory (scripts/fmt/, scripts/lint/, etc.)"
    fi

    if [[ "${name}" =~ [A-Z_] ]]; then
      fail "${script}: filename must be kebab-case (got: ${name})"
    fi

    line1="$(head -1 "${script}")"
    if [[ "${line1}" != "#!/usr/bin/env bash" ]]; then
      fail "${script}: shebang must be #!/usr/bin/env bash"
    fi

    grep -q 'set -o errexit' "${script}"  || fail "${script}: missing set -o errexit"
    grep -q 'set -o nounset' "${script}"  || fail "${script}: missing set -o nounset"
    grep -q 'set -o pipefail' "${script}" || fail "${script}: missing set -o pipefail"

    if ! head -5 "${script}" | tail -4 | grep -q '^#'; then
      fail "${script}: missing header comment in first 5 lines"
    fi

  done < <(find scripts -name '*.sh' -type f | sort)

  if (( failures > 0 )); then
    echo "  ${failures} convention violation(s) found."
    return 1
  fi
  echo "  [done] lint-scripts"
}

main "$@"
