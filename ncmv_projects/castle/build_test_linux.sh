#!/bin/bash

rm -rf build/*
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCASTLE_BUILD_TESTING=ON -DCASTLE_FETCH_GTEST=ON
cmake --build build --parallel
