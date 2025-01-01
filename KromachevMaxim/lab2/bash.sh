#!/bin/bash

set -e

build_dir="build"
fifo="conn_fifo"
socket="conn_socket"
pipe="conn_pipe"

if ! command -v cmake &> /dev/null; then
    echo "CMake не установлен."
    exit 1
fi

echo "CMake уже установлен. Версия:"
cmake --version

echo "Очистка старых файлов..."
rm -rf $build_dir $fifo $socket $pipe

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
cp $pipe ../

echo "Удаление директории сборки..."
cd .. && rm -rf $build_dir
