#!/usr/bin/env bash
# check-comment-ratio.sh — Check that comment ratio meets minimum threshold.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

THRESHOLD=20

echo "==> checking comment ratio..."

totals=$(cloc src/ --exclude-dir=test --not-match-f='_test\.cpp$' --csv --quiet | grep SUM)
comments=$(echo "$totals" | cut -d',' -f4)
code=$(echo "$totals"     | cut -d',' -f5)

total=$((comments + code))
if [ "$total" -eq 0 ]; then
    echo "  ERROR: no source lines found in src/"
    exit 1
fi

ratio=$((comments * 100 / total))

if [ "$ratio" -lt "$THRESHOLD" ]; then
    echo "  FAIL: ${ratio}% (minimum: ${THRESHOLD}%)"
    exit 1
fi

echo "  ${ratio}% (${comments}/${total} lines, minimum: ${THRESHOLD}%)"
echo "  [done] comment-ratio"
