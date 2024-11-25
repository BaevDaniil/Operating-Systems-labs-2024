cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(local_chat VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt6_wrap_cpp(MOC_SOURCES src/host/host_gui.hpp)
qt6_wrap_cpp(MOC_SOURCES_CLIENT src/client/client_gui.hpp)

add_executable(host_mq src/connections/conn_mq.cpp src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_mq PRIVATE USE_MQ_FILE)
target_link_libraries(host_mq Qt6::Widgets)

add_executable(host_sock src/connections/conn_sock.cpp src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_sock PRIVATE USE_SOCK_FILE)
target_link_libraries(host_sock Qt6::Widgets)

add_executable(host_fifo src/connections/conn_fifo.cpp src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_fifo PRIVATE USE_FIFO_FILE)
target_link_libraries(host_fifo Qt6::Widgets)

add_executable(host_shm src/connections/conn_shm.cpp src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_shm PRIVATE USE_FIFO_FILE)
target_link_libraries(host_fifo Qt6::Widgets)

add_executable(client_mq src/connections/conn_mq.cpp src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_mq PRIVATE USE_MQ_FILE)
target_link_libraries(client_mq Qt6::Widgets)

add_executable(client_sock src/connections/conn_sock.cpp src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_sock PRIVATE USE_SOCK_FILE)
target_link_libraries(client_sock Qt6::Widgets)

add_executable(client_fifo src/connections/conn_fifo.cpp src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_fifo PRIVATE USE_FIFO_FILE)
target_link_libraries(client_fifo Qt6::Widgets)

add_executable(client_shm src/connections/conn_shm.cpp src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_shm PRIVATE USE_FIFO_FILE)
target_link_libraries(client_shm Qt6::Widgets)

