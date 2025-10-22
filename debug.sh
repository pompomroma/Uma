#!/bin/bash

# 3D Racing Game Debug Helper Script
# This script helps with debugging the racing game
#
# Usage:
#   ./debug.sh          - Build in debug mode and run with gdb
#   ./debug.sh lldb     - Build in debug mode and run with lldb
#   ./debug.sh valgrind - Build in debug mode and run with valgrind (memory check)
#   ./debug.sh build    - Just build in debug mode without running

set -e  # Exit on error

DEBUGGER="gdb"
ACTION="debug"

if [ "$1" = "lldb" ]; then
    DEBUGGER="lldb"
elif [ "$1" = "valgrind" ]; then
    DEBUGGER="valgrind"
elif [ "$1" = "build" ]; then
    ACTION="build"
elif [ -n "$1" ]; then
    echo "Unknown option: $1"
    echo "Usage: $0 [gdb|lldb|valgrind|build]"
    exit 1
fi

echo "========================================="
echo "3D Racing Game Debug Helper"
echo "========================================="
echo ""

# Build in debug mode
echo "Building in Debug mode..."
./build.sh debug

if [ $? -ne 0 ]; then
    echo "Build failed! Cannot proceed with debugging."
    exit 1
fi

echo ""

# Run based on selected action
if [ "$ACTION" = "build" ]; then
    echo "Debug build complete. You can now debug manually."
    exit 0
fi

# Check if executable exists
if [ ! -f "./build/RacingGame3D" ]; then
    echo "Error: Executable not found at ./build/RacingGame3D"
    exit 1
fi

echo "========================================="
echo "Starting debugger: $DEBUGGER"
echo "========================================="
echo ""

case $DEBUGGER in
    gdb)
        # Check if gdb is installed
        if ! command -v gdb &> /dev/null; then
            echo "Error: gdb is not installed"
            echo "Install with: sudo apt install gdb"
            exit 1
        fi
        
        echo "GDB Quick Reference:"
        echo "  run (r)           - Start the program"
        echo "  break (b) <line>  - Set breakpoint"
        echo "  continue (c)      - Continue execution"
        echo "  next (n)          - Step over"
        echo "  step (s)          - Step into"
        echo "  print (p) <var>   - Print variable"
        echo "  backtrace (bt)    - Show call stack"
        echo "  quit (q)          - Exit debugger"
        echo ""
        echo "Starting gdb..."
        echo ""
        
        cd build
        gdb -ex "set print pretty on" \
            -ex "set print object on" \
            -ex "set print static-members on" \
            ./RacingGame3D
        ;;
        
    lldb)
        # Check if lldb is installed
        if ! command -v lldb &> /dev/null; then
            echo "Error: lldb is not installed"
            echo "Install with: sudo apt install lldb"
            exit 1
        fi
        
        echo "LLDB Quick Reference:"
        echo "  run (r)              - Start the program"
        echo "  breakpoint (b) <loc> - Set breakpoint"
        echo "  continue (c)         - Continue execution"
        echo "  next (n)             - Step over"
        echo "  step (s)             - Step into"
        echo "  print (p) <var>      - Print variable"
        echo "  bt                   - Show call stack"
        echo "  quit (q)             - Exit debugger"
        echo ""
        echo "Starting lldb..."
        echo ""
        
        cd build
        lldb ./RacingGame3D
        ;;
        
    valgrind)
        # Check if valgrind is installed
        if ! command -v valgrind &> /dev/null; then
            echo "Error: valgrind is not installed"
            echo "Install with: sudo apt install valgrind"
            exit 1
        fi
        
        echo "Running with Valgrind (Memory Checker)..."
        echo "This will check for memory leaks and errors."
        echo ""
        
        cd build
        valgrind --leak-check=full \
                 --show-leak-kinds=all \
                 --track-origins=yes \
                 --verbose \
                 --log-file=valgrind-output.txt \
                 ./RacingGame3D
        
        echo ""
        echo "Valgrind output saved to: build/valgrind-output.txt"
        ;;
esac

echo ""
echo "Debugging session ended."
