#!/bin/bash
set -e

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_TESTING=OFF \
        -DENABLE_EXAMPLE=OFF \
        -DBUILD_BENCHMARK=OFF

cmake --build .
cmake --build . --target install