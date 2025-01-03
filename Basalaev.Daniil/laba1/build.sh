#!/bin/bash
mkdir -p build
cd build
cmake ../Demon
cmake --build .

find . -type f -name '*.o' -delete
find . -type f -name '*.cmake' -delete
find . -type f -name 'CMakeCache.txt' -delete
find . -type f -name 'Makefile' -delete
rm -rf CMakeFiles
rm -rf test/CMakeFiles
rm -rf _deps
rm -rf src
