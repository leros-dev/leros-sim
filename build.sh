#!/bin/bash

BUILD_DIRECTORY=build-leros-sim

# Make sure current working directory is the directory of the build script
cd "${0%/*}"

SOURCE_ROOT=$(pwd)

mkdir $BUILD_DIRECTORY
cd $BUILD_DIRECTORY

# Run CMake configuration
cmake -DCMAKE_BUILD_TYPE="Release" $SOURCE_ROOT

# Build
cmake --build .