#!/bin/sh

initial_dir=$(dirname "$(readlink -f "$0")")


if ! command -v cmake &> /dev/null; then
    echo "cmake is not installed. Please install cmake and try again."
    exit 1
fi

if ! command -v qmake6 &> /dev/null; then
    echo "qt6 is not installed. Please install qt6 and try again."
    echo "try this: sudo apt install qt6-base-dev libqt6gui6 libqt6widgets6 libqt6opengl6-dev qt6-wayland"
    exit 1
fi

mkdir -p "$initial_dir/build"

cd "$initial_dir/build" || { echo "Failed to change to the build directory."; exit 1; }

cmake ..
if [ $? -ne 0 ]; then
    echo "Error while running cmake."
    exit 1
fi

make
if [ $? -ne 0 ]; then
    echo "Error while running make."
    exit 1
fi

cd "$initial_dir" || { echo "Failed to exit build directory."; exit 1; }

mv "$initial_dir/build/conn_fifo" "$initial_dir/conn_fifo"

mv "$initial_dir/build/conn_mq" "$initial_dir/conn_mq"

mv "$initial_dir/build/conn_socket" "$initial_dir/conn_socket"

rm -rf "$initial_dir/build"

rm -rf  "$initial_dir/tmp"

mkdir "$initial_dir/tmp"

