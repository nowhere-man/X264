#!/bin/bash
rm -rf build
mkdir -p build
cmake -DCMAKE_BUILD_TYPE=Debug -S./ -B./build
cmake --build ./build --parallel
cmake --install ./build