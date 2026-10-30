#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

OBJ_NAME="bace.o"
CC="${CC:-clang}"

BUILD_TYPE="${BUILD_TYPE:-Debug}"

for arg in "$@"; do
  case $arg in
    --release|-r) BUILD_TYPE="Release" ;;
    *) echo "unknown argument: $arg" >&2; exit 2 ;;
  esac
done

if [[ $BUILD_TYPE != "Debug" && $BUILD_TYPE != "Release" ]]; then
  echo "invalid BUILD_TYPE: $BUILD_TYPE (expected Debug or Release)" >&2
  exit 2
fi

BUILD_PATH="${BUILD_PATH:-./build}"
mkdir -p "$BUILD_PATH"

BUILD_PATH=$(realpath "$BUILD_PATH")
SRC=$(realpath "./src")
INCLUDE=$(realpath "./include")

# extra arguments
read -ra EXTRA_ARGS <<< "${CC_ARGS:-}"

if [[ $BUILD_TYPE == "Release" ]]; then
  OPT_FLAGS=(-O2 -DNDEBUG)
else
  OPT_FLAGS=(-g -Og)
fi

DEFINES=(
  "-D_GNU_SOURCE"
)

COMP_FLAGS=(
  "-I$INCLUDE"
  -Wall -Wextra -Wpedantic -Werror
  -Wno-initializer-overrides
  -std=c11
  -fPIC
)

OBJ_CMD=("$CC" "${DEFINES[@]}" "${OPT_FLAGS[@]}" "${COMP_FLAGS[@]}"
         -c -o "$BUILD_PATH/$OBJ_NAME" "$SRC/bace.c" "${EXTRA_ARGS[@]}")

echo "===== $OBJ_NAME ($BUILD_TYPE) ====="
echo "${OBJ_CMD[*]}"
"${OBJ_CMD[@]}"
