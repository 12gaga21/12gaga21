#!/bin/bash

# Build script for Kaspersky Home Shield Browser
# Usage: ./scripts/build.sh [debug|release]

set -e

BUILD_TYPE=${1:-release}
BUILD_DIR="build_${BUILD_TYPE}"

echo "Building Kaspersky Home Shield Browser in $BUILD_TYPE mode..."

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -DCMAKE_VERBOSE_MAKEFILE=ON \
      ..

# Build
make -j$(nproc)

echo "Build completed successfully!"
echo "Executable location: $BUILD_DIR/KasperskyShieldBrowser"