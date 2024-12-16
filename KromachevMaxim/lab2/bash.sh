#!/bin/bash

set -e

build_dir="build"
fifo="conn_fifo"
socket="conn_socket"

if ! command -v cmake &> /dev/null; then
    echo "CMake не установлен."
    exit 1
fi

echo "CMake уже установлен. Версия:"
cmake --version

echo "Очистка старых файлов..."
rm -rf $build_dir $fifo $socket

echo "Создание директории сборки..."
mkdir $build_dir

cd $build_dir

echo "Запуск cmake..."
cmake .. -DCMAKE_C_FLAGS="-Wall -Werror" -DCMAKE_CXX_FLAGS="-Wall -Werror"

echo "Запуск make..."
make

echo "Сборка завершена успешно."

cp $fifo ../
cp $socket ../

echo "Удаление директории сборки..."
cd .. && rm -rf $build_dir
