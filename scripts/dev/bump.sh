#!/usr/bin/env bash
# bump.sh — Bump the project version in VERSION file.

set -o errexit
set -o nounset
set -o pipefail

PART="${1:-}"
VERSION_FILE="VERSION"

if [[ ! "$PART" =~ ^(major|minor|patch)$ ]]; then
  echo "Usage: make bump PART=<major|minor|patch>"
  exit 1
fi

OLD=$(tr -d '[:space:]' <"$VERSION_FILE")
IFS='.' read -r major minor patch <<<"$OLD"

case "$PART" in
major)
  major=$((major + 1))
  minor=0
  patch=0
  ;;
minor)
  minor=$((minor + 1))
  patch=0
  ;;
patch) patch=$((patch + 1)) ;;
esac

NEW="${major}.${minor}.${patch}"
printf '%s\n' "$NEW" >"$VERSION_FILE"
echo "$OLD -> $NEW"
