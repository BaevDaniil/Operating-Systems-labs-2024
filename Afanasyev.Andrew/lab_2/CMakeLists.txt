cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(local_chat VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt6_wrap_cpp(MOC_SOURCES src/gui/chat_gui.hpp)

set(CONN_FILES
    src/connections/conn_fifo.cpp
    src/connections/conn_mq.cpp
    src/connections/conn_socket.cpp
)


add_executable(conn_mq ${CONN_FILES} src/main.cpp ${MOC_SOURCES})
target_compile_definitions(conn_mq PRIVATE USE_MQ_FILE)
target_link_libraries(conn_mq Qt6::Widgets)

add_executable(conn_fifo ${CONN_FILES} src/main.cpp ${MOC_SOURCES})
target_compile_definitions(conn_fifo PRIVATE USE_FIFO_FILE)
target_link_libraries(conn_fifo Qt6::Widgets)

add_executable(conn_socket ${CONN_FILES} src/main.cpp ${MOC_SOURCES})
target_compile_definitions(conn_socket PRIVATE USE_SOCKET_FILE)
target_link_libraries(conn_socket Qt6::Widgets)
