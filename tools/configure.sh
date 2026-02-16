#!/bin/bash

mkdir -p build && \
cd build && \
conan install . --output-folder=build --build=missing && \
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake