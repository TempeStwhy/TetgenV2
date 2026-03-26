#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  pkg-config \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  freeglut3-dev

echo "OpenGL/GLUT dependencies installed."
