cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
project(local_chat VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(src/includes)
add_subdirectory(src/connections)
add_subdirectory(src/host)

add_library(${PROJECT_NAME}_objs 
    $<TARGET_OBJECTS:includes> 
    $<TARGET_OBJECTS:connections> 
    $<TARGET_OBJECTS:host>
    )

add_executable(${PROJECT_NAME} src/main.cpp)