cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(local_chat VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Werror")
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

# Поиск библиотеки Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Генерация MOC файлов для GUI
qt6_wrap_cpp(MOC_SOURCES src/gui/chat_gui.hpp)
qt6_wrap_cpp(MOC_SOURCES src/gui/client_gui.hpp)

# Список исходных файлов для соединений
set(CONN_FILES
    src/connections_/conn_mq.cpp
    src/connections_/conn_pipe.cpp
    src/connections_/conn_shm.cpp
)

# Основные исполняемые файлы для разных типов соединений

# Для MQ соединения
add_executable(conn_mq 
    ${CONN_FILES} 
    src/main.cpp
    src/client_/client.cpp
    src/gui/client_gui.cpp  # Добавляем новый файл
    ${MOC_SOURCES}
)
target_compile_definitions(conn_mq PRIVATE USE_MQ_FILE)
target_link_libraries(conn_mq Qt6::Widgets)

# Для PIPE соединения
add_executable(conn_pipe 
    ${CONN_FILES} 
    src/main.cpp
    src/client_/client.cpp
    src/gui/client_gui.cpp  # Добавляем новый файл
    ${MOC_SOURCES}
)
target_compile_definitions(conn_pipe PRIVATE USE_PIPE_FILE)
target_link_libraries(conn_pipe Qt6::Widgets)

# Для SHM соединения
add_executable(conn_shm 
    ${CONN_FILES} 
    src/main.cpp
    src/client_/client.cpp
    src/gui/client_gui.cpp  # Добавляем новый файл
    ${MOC_SOURCES}
)
target_compile_definitions(conn_shm PRIVATE USE_SHM_FILE)
target_link_libraries(conn_shm Qt6::Widgets)
