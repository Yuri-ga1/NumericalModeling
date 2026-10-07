#!/bin/bash

set -e

cmake -S . -B build
cmake --build ./build

./build/integrals

python plotter.py
