#!/bin/bash

rm -rf build/*
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCASTLE_BUILD_SAMPLES=ON
cmake --build build --parallel
