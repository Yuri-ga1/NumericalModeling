#!/bin/bash

set -e

cmake -S . -B build
cmake --build ./build

./build/integrals

python integration_plotter.py
python treads_plotter.py
python adaptive_plotter.py
