cmake_minimum_required(VERSION 3.5.0)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")

# Определение проекта
project(daemon VERSION 0.1.0 LANGUAGES C CXX)

# Добавление исполняемого файла
add_executable(daemon config.cpp daemon.cpp main.cpp)
