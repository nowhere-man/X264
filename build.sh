#!/bin/bash
rm -rf build
mkdir -p build
cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -S./ -B./build -G "Unix Makefiles" --no-warn-unused-cli
cmake --build ./build --parallel
cmake --install ./build