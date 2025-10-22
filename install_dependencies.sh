#!/bin/bash

# 3D Racing Game Dependencies Installation Script
# This script installs all required dependencies for the racing game

echo "Installing dependencies for 3D Racing Game..."

# Update package list
echo "Updating package list..."
sudo apt update

# Install build tools
echo "Installing build tools..."
sudo apt install -y build-essential cmake git

# Install OpenGL development libraries
echo "Installing OpenGL development libraries..."
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev

# Install GLFW3
echo "Installing GLFW3..."
sudo apt install -y libglfw3-dev

# Install GLEW
echo "Installing GLEW..."
sudo apt install -y libglew-dev

# Install additional libraries
echo "Installing additional libraries..."
sudo apt install -y libglm-dev libassimp-dev

# Install development tools
echo "Installing development tools..."
sudo apt install -y gdb valgrind

echo "Dependencies installation complete!"
echo "You can now run ./build.sh to build the project"