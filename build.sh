rm -rf build
mkdir -p build

./configure --prefix=./build --enable-shared --bit-depth=8 --chroma-format=420 --disable-opencl --disable-interlaced --enable-debug

make clean

make -j 20

make install