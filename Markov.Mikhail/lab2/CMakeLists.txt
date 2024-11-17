cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
project(local_chat VERSION 0.1.0 LANGUAGES C CXX)

add_executable(client_fifo src/connections/conn_fifo.cpp src/client/client_fifo.cpp)
add_executable(host_fifo src/connections/conn_fifo.cpp src/host/host_fifo.cpp)

add_executable(client_mq src/connections/conn_mq.cpp src/client/client_mq.cpp)
add_executable(host_mq src/connections/conn_mq.cpp src/host/host_mq.cpp)

# add_executable(host_mq src/connections/conn_mq.cpp src/connections/conn_fifo.cpp src/host/host_main.cpp)
# target_compile_definitions(host_mq PRIVATE USE_MQ_FILE)
# add_executable(host_fifo src/connections/conn_mq.cpp src/connections/conn_fifo.cpp src/host/host_main.cpp)
# target_compile_definitions(host_fifo PRIVATE USE_FIFO_FILE)
