#!/bin/bash

rm -rf build
mkdir -p build

./configure \
    --prefix=./build \
    --enable-shared \
    --bit-depth=8 \
    --chroma-format=420 \
    --disable-opencl \
    --disable-interlaced \
    --disable-asm \
    --disable-thread \
    --disable-avs \
    --disable-swscale \
    --disable-lavf \
    --disable-ffms \
    --disable-gpac \
    --disable-lsmash \
    --enable-debug \

make clean

make -j 20

make install