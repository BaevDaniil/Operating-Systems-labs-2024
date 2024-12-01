cmake_minimum_required(VERSION 3.5)

project(Library)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)

add_subdirectory(src)
add_subdirectory(test)

add_executable(host_fifo host_fifo.cpp)
target_link_libraries(host_fifo LibraryLib)

add_executable(host_sock host_sock.cpp)
target_link_libraries(host_sock LibraryLib)
