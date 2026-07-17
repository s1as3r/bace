#!/usr/bin/env bash
OBJ_NAME="bace.o"
CC="clang"
CXX="clang++"

BUILD_PATH="${BUILD_PATH:-$(realpath './build/')}"
SRC=$(realpath "./src/")
INCLUDE=$(realpath "./include/")

# debug flags
DEBUG=(
    "-g"
)

# compile time defines
DEFINES=(
    "-D_DEFAULT_SOURCE"
)

# linux platform libraries
LIBS=(
    # "-L$BUILD_PATH"
    # "-lm"
)

# compiler flags
COMP_FLAGS=(
    "-I$INCLUDE"
    "-Wall"
    "-Wextra"
    "-Wpedantic"
    "-Werror"
    "-Wno-initializer-overrides"
    "-Og"
    "-std=c11"
    "-fPIC"
)

# build commands
OBJ_CMD=("$CC" "${DEFINES[@]}" "${DEBUG[@]}" "${COMP_FLAGS[@]}" \
         "-c" "-o" "$OBJ_NAME" "$SRC/bace.c" "$VMA_OBJ" "${LIBS[@]}")

OBJ_CMD_STR=$(IFS=' '; echo "${OBJ_CMD[*]}")


# create build directory if it doesn't exist
if [[ ! -d "$BUILD_PATH" ]]; then
    echo "created build directory"
    mkdir -p "$BUILD_PATH"
fi

cd "$BUILD_PATH" || exit
echo "===== $OBJ_NAME ====="
echo "$OBJ_CMD_STR"
eval "$OBJ_CMD_STR"
cd ..
