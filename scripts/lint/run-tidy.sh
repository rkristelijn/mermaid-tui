#!/usr/bin/env bash
# run-tidy.sh — Run clang-tidy on source files.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

CLANG_TIDY="${CLANG_TIDY:-$(command -v clang-tidy 2>/dev/null || echo /opt/homebrew/opt/llvm/bin/clang-tidy)}"
FULL=false
[[ "${1-}" == "--full" ]] && FULL=true

main() {
  if [[ "${FULL}" == true ]]; then
    echo "==> make tidy (full mode)"
    find src -name '*.cpp' -print0 \
      | xargs -0 "${CLANG_TIDY}" --config-file=.config/.clang-tidy -- -std=c++20 -I src/ 2>&1 \
      | grep "warning:" && exit 1 || true
  else
    echo "==> make tidy (smart incremental mode)"
    local branch diff_base files
    branch="$(git rev-parse --abbrev-ref HEAD)"
    diff_base="$( [[ "${branch}" == "main" ]] && echo "HEAD^" || echo "origin/main" )"
    files="$(git diff --name-only "${diff_base}" | grep '\.cpp$' | grep '^src/' || true)"
    local headers
    headers="$(git diff --name-only "${diff_base}" | grep '\.h$' | grep '^src/' || true)"

    if [[ -n "${headers}" ]]; then
      local header_dirs
      header_dirs="$(echo "${headers}" | xargs -n1 dirname | sort -u)"
      for hdir in ${header_dirs}; do
        files="$(printf '%s\n%s' "${files}" "$(find "${hdir}" -maxdepth 1 -name '*.cpp' 2>/dev/null)" | sort -u)"
      done
      files="$(echo "${files}" | sed '/^$/d')"
    fi

    if [[ -z "${files}" ]]; then
      echo "  [skip] no changed files vs ${diff_base}"
    else
      echo "  [checking] $(echo "${files}" | wc -w | tr -d ' ') files"
      # shellcheck disable=SC2086
      ${CLANG_TIDY} --config-file=.config/.clang-tidy ${files} -- -std=c++20 -I src/ 2>&1 \
        | grep "warning:" && exit 1 || true
    fi
  fi
  echo "  [done] tidy"
}

main "$@"
