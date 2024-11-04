cmake_minimum_required(VERSION 3.10)

project(LocalChatApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Set the path for Qt6
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

# Include directories
include_directories(include)

# Find Qt6 package
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Host source files
set(HOST_SRC_FILES
  src/host.cpp
  src/conn_queue.cpp
  src/conn_fifo.cpp
  src/conn_socket.cpp
)

# Client source files
set(CLIENT_SRC_FILES
  src/client.cpp
  src/conn_queue.cpp
  src/conn_fifo.cpp
  src/conn_socket.cpp
)

# Add host executable
add_executable(host ${HOST_SRC_FILES})

# Add client executable
add_executable(client ${CLIENT_SRC_FILES})

# Set output directories for host and client
set_target_properties(host PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add subdirectory for GUI
add_subdirectory(src/gui)

# Custom target to build all
add_custom_target(all_targets DEPENDS host client chat_gui)
