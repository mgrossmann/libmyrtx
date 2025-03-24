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
BUILD_DOCS=0
DOCS_HTML=0
DOCS_PDF=0
DOCS_SETUP_ENV=0
DOCS_CLEAN=0
INSTALL_PREFIX="/usr/local"
BUILD_DIR="build"
DOCS_DIR="docs"
VENV_DIR=".venv"

# Help function
show_help() {
    echo "Usage: $0 [options]"
    echo
    echo "Build options:"
    echo "  -h, --help               Show this help message"
    echo "  -t, --type TYPE          Set build type (Debug or Release) [default: Debug]"
    echo "  -c, --clean              Clean the build directory before building"
    echo "  --test                   Run tests after building"
    echo "  --no-examples            Don't build examples"
    echo "  -i, --install            Install the library after building"
    echo "  --prefix PATH            Set installation path [default: /usr/local]"
    echo "  --build-dir DIR          Set build directory [default: build]"
    echo
    echo "Documentation options:"
    echo "  --docs                   Build documentation (HTML format)"
    echo "  --docs-html              Build HTML documentation"
    echo "  --docs-pdf               Build PDF documentation"
    echo "  --docs-setup-env         Setup Python virtual environment for documentation"
    echo "  --docs-clean             Clean documentation build directory"
    echo "  --docs-all               Setup environment and build all documentation formats"
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
        --docs)
            BUILD_DOCS=1
            DOCS_HTML=1
            shift
            ;;
        --docs-html)
            BUILD_DOCS=1
            DOCS_HTML=1
            shift
            ;;
        --docs-pdf)
            BUILD_DOCS=1
            DOCS_PDF=1
            shift
            ;;
        --docs-setup-env)
            DOCS_SETUP_ENV=1
            shift
            ;;
        --docs-clean)
            DOCS_CLEAN=1
            shift
            ;;
        --docs-all)
            BUILD_DOCS=1
            DOCS_HTML=1
            DOCS_PDF=1
            DOCS_SETUP_ENV=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Check if we're only working with docs
DOCS_ONLY=0
if [[ $BUILD_DOCS -eq 1 || $DOCS_SETUP_ENV -eq 1 || $DOCS_CLEAN -eq 1 ]] && 
   [[ $CLEAN -eq 0 && $INSTALL -eq 0 && $RUN_TESTS -eq 0 ]]; then
    DOCS_ONLY=1
fi

# Function to setup Python virtual environment
setup_docs_env() {
    echo "Setting up Python virtual environment for documentation..."
    
    if [ ! -d "$VENV_DIR" ]; then
        python3 -m venv "$VENV_DIR"
    fi
    
    # Activate virtual environment and install dependencies
    source "$VENV_DIR/bin/activate"
    pip install --upgrade pip
    pip install -r "$DOCS_DIR/requirements.txt"
    
    echo "Documentation environment setup completed!"
}

# Function to build documentation
build_docs() {
    echo "Building documentation..."
    
    # Activate virtual environment
    source "$VENV_DIR/bin/activate"
    
    # Create directories that might be missing
    mkdir -p "$DOCS_DIR/source/_static" "$DOCS_DIR/source/_templates"
    
    # Create Doxygen documentation
    if [ -f "$DOCS_DIR/Doxyfile" ]; then
        echo "Generating Doxygen XML..."
        cd "$DOCS_DIR" && doxygen Doxyfile
        cd ..
    fi
    
    # Build HTML documentation if requested
    if [ $DOCS_HTML -eq 1 ]; then
        echo "Building HTML documentation..."
        echo "Debugging: Current directory is $(pwd)"
        echo "Debugging: Checking directories in $DOCS_DIR"
        ls -la "$DOCS_DIR"
        echo "Debugging: Checking directories in $DOCS_DIR/source"
        ls -la "$DOCS_DIR/source"
        
        set +e  # Don't exit on error for better error reporting
        cd "$DOCS_DIR" && PYTHONPATH=$(pwd) python -m sphinx -v -b html source build/html 2>&1
        SPHINX_RESULT=$?
        cd ..
        
        if [ $SPHINX_RESULT -eq 0 ]; then
            echo "HTML documentation generated in $DOCS_DIR/build/html/"
        else
            echo "ERROR: Failed to build HTML documentation. See errors above."
            if [ -f "$DOCS_DIR/build/html/errors.log" ]; then
                echo "Detailed error log:"
                cat "$DOCS_DIR/build/html/errors.log"
            fi
            # Only exit if we're only building docs, otherwise continue with other tasks
            if [ $DOCS_ONLY -eq 1 ]; then
                exit 1
            fi
        fi
        set -e  # Restore exit on error
    fi
    
    # Build PDF documentation if requested
    if [ $DOCS_PDF -eq 1 ]; then
        echo "Building PDF documentation..."
        
        # Check for required tools
        check_latex_deps
        
        set +e  # Don't exit on error for better error reporting
        cd "$DOCS_DIR" && PYTHONPATH=$(pwd) python -m sphinx -v -b latex source build/latex 2>&1
        SPHINX_RESULT=$?
        
        if [ $SPHINX_RESULT -eq 0 ]; then
            # Only try to build PDF if sphinx succeeded
            if command -v latexmk &> /dev/null; then
                echo "Running LaTeX build..."
                cd build/latex && make 2>&1
                LATEX_RESULT=$?
                cd ../..
                
                if [ $LATEX_RESULT -eq 0 ]; then
                    echo "PDF documentation generated in $DOCS_DIR/build/latex/"
                else
                    echo "ERROR: Failed to build PDF with LaTeX. See errors above."
                fi
            else
                echo "WARNING: 'latexmk' not found, skipping PDF generation."
                echo "LaTeX files are available in $DOCS_DIR/build/latex/"
            fi
        else
            echo "ERROR: Failed to build LaTeX documentation. See errors above."
        fi
        
        cd ..
        set -e  # Restore exit on error
    fi
}

# Function to check if LaTeX dependencies are installed
check_latex_deps() {
    local missing_deps=()
    
    # Check for basic LaTeX commands
    for cmd in pdflatex latexmk; do
        if ! command -v $cmd &> /dev/null; then
            missing_deps+=($cmd)
        fi
    done
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        echo "WARNING: Some LaTeX dependencies are missing for PDF generation:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        
        if [ -f /etc/os-release ]; then
            . /etc/os-release
            case "$ID" in
                ubuntu|debian)
                    echo "To install the required packages, run:"
                    echo "  sudo apt-get install texlive-latex-recommended texlive-latex-extra texlive-fonts-recommended latexmk"
                    ;;
                fedora|centos|rhel)
                    echo "To install the required packages, run:"
                    echo "  sudo dnf install texlive-scheme-basic texlive-latex-extra latexmk"
                    ;;
                *)
                    echo "Please install a LaTeX distribution and latexmk for PDF generation."
                    ;;
            esac
        else
            echo "Please install a LaTeX distribution and latexmk for PDF generation."
        fi
        
        if [ $DOCS_ONLY -eq 1 ]; then
            echo "Continuing with HTML documentation only..."
        fi
    fi
}

# Clean documentation if requested
if [ $DOCS_CLEAN -eq 1 ]; then
    echo "Cleaning documentation build directory..."
    rm -rf "$DOCS_DIR/build"
    rm -rf "$DOCS_DIR/doxygen_output"
    
    if [ $DOCS_ONLY -eq 1 ]; then
        echo "Documentation cleanup completed!"
        exit 0
    fi
fi

# Setup documentation environment if requested
if [ $DOCS_SETUP_ENV -eq 1 ]; then
    setup_docs_env
    
    if [ $DOCS_ONLY -eq 1 ] && [ $BUILD_DOCS -eq 0 ]; then
        echo "Documentation environment setup completed!"
        exit 0
    fi
fi

# Build documentation if requested
if [ $BUILD_DOCS -eq 1 ]; then
    # Check if virtual environment exists, if not, set it up
    if [ ! -d "$VENV_DIR" ]; then
        setup_docs_env
    fi
    
    build_docs
    
    if [ $DOCS_ONLY -eq 1 ]; then
        exit 0
    fi
fi

# If we get here, we're building the library

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