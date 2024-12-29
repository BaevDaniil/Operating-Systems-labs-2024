#!/bin/sh

init_dir=$(dirname "$(readlink -f "$0")")

#check cmake
if ! command -v cmake &> /dev/null; then
    echo "cmake is not installed. Please install cmake and try again."
    exit 1
fi

# build dir
mkdir -p "$init_dir/build"

cd "$init_dir/build" || { echo "Failed to change to the build directory."; exit 1; }

# build project
cmake ..
if [ $? -ne 0 ]; then
    echo "Error while running cmake."
    exit 1
fi

# also build project
make
if [ $? -ne 0 ]; then
    echo "Error while running make."
    exit 1
fi

# back
cd "$init_dir" || { echo "Failed to exit build directory."; exit 1; }

# move exe to init folder
mv "$init_dir/build/app_daemon" "$init_dir"

# exec
sudo "$init_dir/app_daemon" "$init_dir/config.txt"

# remove buld dir
sudo rm -rf "$init_dir/build/"