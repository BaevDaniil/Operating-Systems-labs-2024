cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(IPC_Lab VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

qt6_wrap_cpp(MOC_SOURCES ${HEADERS})

include_directories(${Qt6Widgets_INCLUDE_DIRS} ${Qt6Core_INCLUDE_DIRS} "src/connections" "src/host" "src/client")

add_executable(host ${SOURCES} ${HEADERS} ${MOC_SOURCES})
add_executable(client ${SOURCES} ${HEADERS} ${MOC_SOURCES})

target_link_libraries(host Qt6::Widgets Qt6::Core)
target_link_libraries(client Qt6::Widgets Qt6::Core)
