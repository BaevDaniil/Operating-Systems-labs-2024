cmake_minimum_required(VERSION 3.14)

project(local_chat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(FIFO_FILES
    src/host_fifo.cpp
    src/client_fifo.cpp
    src/conn_fifo.cpp
)

set(GUI_FILES
    src/host_window.cpp
    src/client_window.cpp
    src/input_dialog.cpp
)

add_executable(conn_fifo
    ${FIFO_FILES}
    ${GUI_FILES}
)

target_link_libraries(conn_fifo PRIVATE Qt6::Widgets)
