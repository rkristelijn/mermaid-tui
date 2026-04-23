#!/usr/bin/env bash
# build-tests.sh — Compile all test_*.cpp files.

set -o errexit
set -o nounset
set -o pipefail
if [[ "${TRACE-0}" == "1" ]]; then set -o xtrace; fi

CXX="${CXX:-g++}"
CXXFLAGS="${CXXFLAGS:--Wall -Wextra -std=c++20 -I src/}"
SRCS="${SRCS:-}"

mkdir -p build
for t in $(find src -name 'test_*.cpp'); do
  name=$(basename "$t" .cpp)
  lib_srcs=$(echo "$SRCS" | tr ' ' '\n' | grep -v main.cpp)
  $CXX $CXXFLAGS -o "build/$name" "$t" $lib_srcs -lm
done
