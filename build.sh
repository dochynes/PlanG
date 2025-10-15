#!/usr/bin/env bash


cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF \
  -DBUILD_BENCHMARKS=OFF


cmake --build build -j

echo "Running demo:"
./build/ggi_demo