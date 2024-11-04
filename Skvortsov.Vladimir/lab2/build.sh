#!/bin/bash

# Остановить скрипт при ошибке
set -e

# Директория для сборки
BUILD_DIR="build"

# Очистка предыдущих сборок
echo "Очистка предыдущих сборок..."
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR

# Переход в директорию сборки
cd $BUILD_DIR

# Генерация Makefile с помощью CMake
echo "Генерация Makefile с помощью CMake..."
cmake ..

# Компиляция проекта
echo "Компиляция проекта..."
make

echo "Сборка завершена успешно!"
