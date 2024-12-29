#!/bin/sh

# Получение пути до текущего каталога
initial_dir=$(dirname "$(readlink -f "$0")")

# Проверка наличия cmake
CMAKE_PATH=$(which cmake)

if [ -z "$CMAKE_PATH" ]; then
    echo "cmake is not installed. Please install cmake and try again."
    exit 1
else
    echo "Using cmake at $CMAKE_PATH"
fi

# Создание папки build
mkdir -p "$initial_dir/build"

# Переход в папку build
cd "$initial_dir/build" || { echo "Failed to change to the build directory."; exit 1; }

# Запуск cmake и make
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

# Возвращение в начальную директорию и запуск демона
cd "$initial_dir" || { echo "Failed to exit build directory."; exit 1; }

sudo "$initial_dir/build/daemon" "$initial_dir/config.txt"

# Очистка после выполнения
sudo rm -rf "$initial_dir/build/daemon"
