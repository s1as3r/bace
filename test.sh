#!/usr/bin/env bash
EXE_PREFIX="test_"
CC="clang"
CXX="clang++"

BUILD_PATH="${BUILD_PATH:-$(realpath ./build/tests)}"
TESTS=$(realpath "./tests/")
INCLUDES=$(realpath "./include/")
ACUTEST_PATH=$(realpath "./external/acutest/include/")

BACE_LIB="$BUILD_PATH/bace.o"
BACE_BUILD_SH=$(realpath "./build.sh")

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
    "$BACE_LIB"
)

# compiler flags
COMP_FLAGS=(
    "-I$ACUTEST_PATH" "-I$INCLUDES"
    "-Wall"
    "-Wextra"
    "-Wpedantic"
    "-Werror"
    "-Wno-initializer-overrides"
    "-Og"
    "-std=c11"
)

build_bace() {
    BUILD_PATH="$BUILD_PATH" $BACE_BUILD_SH
}

no_build=false
no_run=false
build_bace=false
for arg in "$@"; do
  case $arg in
    --no-build|-B)
      no_build=true
      ;;
    --no-run|-R)
      no_run=true
      ;;
    --build-bace|-b)
      build_bace=true
      ;;
  esac
done

# create build directory if it doesn't exist
if [[ ! -d "$BUILD_PATH" ]]; then
    echo "created build directory"
    mkdir -p "$BUILD_PATH"
fi

if [[ ! -f "$BACE_LIB" || $build_bace == "true" ]]; then
    echo "=== building bace ==="
    if ! build_bace; then
        echo "building bace failed";
        exit 1;
    fi
fi

cd "$BUILD_PATH" || exit
for file in "$TESTS/"*.c; do
    fname=$(basename $file)
    exe_name="$EXE_PREFIX${fname%.*}"
    exe_cmd=("$CC" "${DEFINES[@]}" "${DEBUG[@]}" "${COMP_FLAGS[@]}" \
    "-o" "$exe_name" "$file" "${LIBS[@]}")

    exe_cmd_str=$(IFS=' '; echo "${exe_cmd[*]}")
    echo "===== $exe_name ====="
    if [[ $no_build == "false" ]]; then
        echo "$exe_cmd_str"
        if ! "${exe_cmd[@]}"; then
            echo "failed building $exe_name"
        fi
    fi
    if [[ $no_run == "false" ]]; then
        "./$exe_name"
    fi
done
cd ..
