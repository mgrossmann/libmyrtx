#!/bin/bash
# build.sh - Build script for the libmyrtx project
# 
# This script automates the build process for the libmyrtx library
# with various configuration options.

set -e # Exit on error

BUILD_TYPE="Debug"
RUN_TESTS=0
BUILD_EXAMPLES=1
CLEAN=0
INSTALL=0
INSTALL_PREFIX="/usr/local"
BUILD_DIR="build"

# Help function
show_help() {
    echo "Usage: $0 [options]"
    echo
    echo "Options:"
    echo "  -h, --help               Show this help message"
    echo "  -t, --type TYPE          Set build type (Debug or Release) [default: Debug]"
    echo "  -c, --clean              Clean the build directory before building"
    echo "  --test                   Run tests after building"
    echo "  --no-examples            Don't build examples"
    echo "  -i, --install            Install the library after building"
    echo "  --prefix PATH            Set installation path [default: /usr/local]"
    echo "  --build-dir DIR          Set build directory [default: build]"
    echo
}

# Process parameters
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN=1
            shift
            ;;
        --test)
            RUN_TESTS=1
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=0
            shift
            ;;
        -i|--install)
            INSTALL=1
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Check build type
if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
    echo "Invalid build type: $BUILD_TYPE. Valid values are 'Debug' or 'Release'."
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Clean build directory if requested
if [ $CLEAN -eq 1 ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"/*
fi

# Change to build directory
cd "$BUILD_DIR"

# CMake configuration
echo "Configuring CMake for $BUILD_TYPE build..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ $BUILD_EXAMPLES -eq 0 ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DMYRTX_BUILD_EXAMPLES=OFF"
fi

# Set installation path if installation is enabled
if [ $INSTALL -eq 1 ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
fi

cmake $CMAKE_ARGS ..

# Perform build
echo "Building libmyrtx..."
cmake --build . -- -j$(nproc)

# Run tests if requested
if [ $RUN_TESTS -eq 1 ]; then
    echo "Running tests..."
    ctest --output-on-failure
fi

# Install if requested
if [ $INSTALL -eq 1 ]; then
    echo "Installing libmyrtx to $INSTALL_PREFIX..."
    cmake --install .
fi

echo "Build completed!"

if [ $BUILD_EXAMPLES -eq 1 ]; then
    echo "Example programs are located in $BUILD_DIR/examples/"
fi

cd .. 