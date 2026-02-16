#!/bin/bash


TARGET_BUILD_SYSTEM="Unix Makefiles"

if command -v ninja >/dev/null 2>&1; then
    TARGET_BUILD_SYSTEM="Ninja";
fi

echo "Configuring with ${TARGET_BUILD_SYSTEM} at $(pwd)"

mkdir -p build && \
conan install . --output-folder=build --build=missing -c tools.cmake.cmaketoolchain:generator="${TARGET_BUILD_SYSTEM}" && \
cmake --preset conan-release
