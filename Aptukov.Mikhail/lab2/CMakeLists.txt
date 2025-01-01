cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(local_chat VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt6_wrap_cpp(MOC_SOURCES src/host/host_gui.hpp)
qt6_wrap_cpp(MOC_SOURCES_CLIENT src/client/client_gui.hpp)

set(CONN_FILES
    src/connections/conn_fifo.cpp
    src/connections/conn_mq.cpp
    src/connections/conn_socket.cpp
)


add_executable(host_mq ${CONN_FILES} src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_mq PRIVATE MQ)
target_link_libraries(host_mq Qt6::Widgets)

add_executable(host_fifo ${CONN_FILES} src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_fifo PRIVATE FIFO)
target_link_libraries(host_fifo Qt6::Widgets)

add_executable(host_socket ${CONN_FILES} src/host/host_main.cpp ${MOC_SOURCES})
target_compile_definitions(host_socket PRIVATE SOCKET)
target_link_libraries(host_socket Qt6::Widgets)

add_executable(client_mq ${CONN_FILES} src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_mq PRIVATE MQ)
target_link_libraries(client_mq Qt6::Widgets)

add_executable(client_fifo ${CONN_FILES} src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_fifo PRIVATE FIFO)
target_link_libraries(client_fifo Qt6::Widgets)

add_executable(client_socket ${CONN_FILES} src/client/client_main.cpp ${MOC_SOURCES_CLIENT})
target_compile_definitions(client_socket PRIVATE SOCKET)
target_link_libraries(client_socket Qt6::Widgets)
