#!/bin/bash

# CMake Build Test Script for x264
set -e

echo "======================================"
echo "x264 CMake Build Verification Script"
echo "======================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build_test"
INSTALL_DIR="${SCRIPT_DIR}/install_test"

# Clean previous test builds
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous test builds..."
    rm -rf "$BUILD_DIR"
fi

if [ -d "$INSTALL_DIR" ]; then
    rm -rf "$INSTALL_DIR"
fi

# Test counter
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    shift
    local cmake_args="$@"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo ""
    echo "--------------------------------------"
    echo "Test $TOTAL_TESTS: $test_name"
    echo "--------------------------------------"
    echo "CMake args: $cmake_args"

    local test_build_dir="${BUILD_DIR}/test_${TOTAL_TESTS}"
    local test_install_dir="${INSTALL_DIR}/test_${TOTAL_TESTS}"

    mkdir -p "$test_build_dir"

    if (cd "$test_build_dir" && \
        cmake "$SCRIPT_DIR" $cmake_args \
            -DCMAKE_INSTALL_PREFIX="$test_install_dir" \
            > cmake_output.log 2>&1 && \
        cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4) \
            > build_output.log 2>&1); then

        echo -e "${GREEN}✓ PASSED${NC}: $test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo -e "${RED}✗ FAILED${NC}: $test_name"
        echo "Check logs in: $test_build_dir"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# Detect system
SYSTEM=$(uname -s)
ARCH=$(uname -m)

echo "System: $SYSTEM"
echo "Architecture: $ARCH"
echo ""

# Test 1: Default build (all bit depths, all chroma formats)
run_test "Default build (8+10 bit, all chroma)" \
    -DCMAKE_BUILD_TYPE=Release

# Test 2: 8-bit only
run_test "8-bit only build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DX264_BIT_DEPTH=8

# Test 3: 10-bit only
run_test "10-bit only build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DX264_BIT_DEPTH=10

# Test 4: Shared library build
run_test "Shared library build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON

# Test 5: Static library build (no CLI)
run_test "Static library only (no CLI)" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DENABLE_CLI=OFF

# Test 6: No assembly optimizations
run_test "Build without assembly" \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_ASM=OFF

# Test 7: No OpenCL
run_test "Build without OpenCL" \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_OPENCL=OFF

# Test 8: Debug build
run_test "Debug build" \
    -DCMAKE_BUILD_TYPE=Debug

# Test 9: RelWithDebInfo build
run_test "RelWithDebInfo build" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Test 10: MinSizeRel build
run_test "MinSizeRel build" \
    -DCMAKE_BUILD_TYPE=MinSizeRel

# Test 11: LTO enabled (if supported)
if [ "$SYSTEM" = "Linux" ] || [ "$SYSTEM" = "Darwin" ]; then
    run_test "Link-time optimization (LTO)" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_LTO=ON
fi

# Test 12: Chroma format 420 only
run_test "Chroma 420 only" \
    -DCMAKE_BUILD_TYPE=Release \
    -DX264_CHROMA_FORMAT=420

# Test 13: No threading
run_test "Single-threaded build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_THREAD=OFF

# Test 14: All external libraries disabled
run_test "No external libraries" \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_AVS=off \
    -DENABLE_SWSCALE=off \
    -DENABLE_LAVF=off \
    -DENABLE_FFMS=off \
    -DENABLE_GPAC=off \
    -DENABLE_LSMASH=off

echo ""
echo "======================================"
echo "Test Summary"
echo "======================================"
echo "Total tests: $TOTAL_TESTS"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
if [ $FAILED_TESTS -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED_TESTS${NC}"
else
    echo "Failed: $FAILED_TESTS"
fi
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed. Please check the logs.${NC}"
    exit 1
fi
