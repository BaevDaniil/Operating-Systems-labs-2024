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

# Host and Client source files for FIFO
set(HOST_FIFO_SRC_FILES
  src/host_fifo.cpp
  src/conn_fifo.cpp
)

set(CLIENT_FIFO_SRC_FILES
  src/client_fifo.cpp
  src/conn_fifo.cpp
)

# Host and Client source files for Queue
set(HOST_QUEUE_SRC_FILES
  src/host_queue.cpp
  src/conn_queue.cpp
)

set(CLIENT_QUEUE_SRC_FILES
  src/client_queue.cpp
  src/conn_queue.cpp
)

# Host and Client source files for Socket
set(HOST_SOCKET_SRC_FILES
  src/host_socket.cpp
  src/conn_socket.cpp
)

set(CLIENT_SOCKET_SRC_FILES
  src/client_socket.cpp
  src/conn_socket.cpp
)

# Add executables for FIFO communication
add_executable(host_fifo ${HOST_FIFO_SRC_FILES})
add_executable(client_fifo ${CLIENT_FIFO_SRC_FILES})

# Add executables for Queue communication
add_executable(host_queue ${HOST_QUEUE_SRC_FILES})
add_executable(client_queue ${CLIENT_QUEUE_SRC_FILES})

# Add executables for Socket communication
add_executable(host_socket ${HOST_SOCKET_SRC_FILES})
add_executable(client_socket ${CLIENT_SOCKET_SRC_FILES})

# Set output directories for all executables
set_target_properties(host_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(host_queue PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client_queue PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(host_socket PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client_socket PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add subdirectory for GUI
add_subdirectory(src/gui)

# Custom target to build all
add_custom_target(all_targets DEPENDS host_fifo client_fifo host_queue client_queue host_socket client_socket chat_gui)
