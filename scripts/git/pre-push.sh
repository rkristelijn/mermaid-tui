#!/usr/bin/env bash
# pre-push — Run comprehensive checks before pushing.

set -o errexit
set -o nounset
set -o pipefail

exec bash scripts/git/prepush-check.sh
