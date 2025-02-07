cmake_minimum_required(VERSION 3.10)

# Проект
project(LocalChatApp)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Путь для поиска Qt6
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

# Находим Qt6 и нужные компоненты
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Пути для исходников и заголовков
include_directories(src/connections src/host src/client)

# Определяем пути до заголовочных файлов и используем moc с ними
qt6_wrap_cpp(MOC_SOURCES
  src/host/host_gui.hpp
  src/client/client_gui.hpp
)

# Определяем исходники для FIFO
set(HOST_FIFO_SRC_FILES
  src/host/host_fifo.cpp
  src/connections/conn_fifo.cpp
  src/host/host_gui.cpp
)

set(CLIENT_FIFO_SRC_FILES
  src/client/client_fifo.cpp
  src/connections/conn_fifo.cpp
  src/client/client_gui.cpp
)

# Определяем исходники для очередей сообщений
set(HOST_QUEUE_SRC_FILES
  src/host/host_mq.cpp
  src/connections/conn_mq.cpp
  src/host/host_gui.cpp
)

set(CLIENT_QUEUE_SRC_FILES
  src/client/client_mq.cpp
  src/connections/conn_mq.cpp
  src/client/client_gui.cpp
)

# Определяем исходники для разделяемой памяти
set(HOST_SHM_SRC_FILES
  src/host/host_seg.cpp
  src/connections/conn_seg.cpp
  src/host/host_gui.cpp
)

set(CLIENT_SHM_SRC_FILES
  src/client/client_seg.cpp
  src/connections/conn_seg.cpp
  src/client/client_gui.cpp
)

# Определяем цели для FIFO
add_executable(host_fifo ${HOST_FIFO_SRC_FILES} ${MOC_SOURCES})
add_executable(client_fifo ${CLIENT_FIFO_SRC_FILES} ${MOC_SOURCES})

# Определяем цели для очередей сообщений
add_executable(host_mq ${HOST_QUEUE_SRC_FILES} ${MOC_SOURCES})
add_executable(client_mq ${CLIENT_QUEUE_SRC_FILES} ${MOC_SOURCES})

# Определяем цели для разделяемой памяти
add_executable(host_seg ${HOST_SHM_SRC_FILES} ${MOC_SOURCES})
add_executable(client_seg ${CLIENT_SHM_SRC_FILES} ${MOC_SOURCES})

# Привязка библиотек Qt6
target_link_libraries(host_fifo Qt6::Widgets)
target_link_libraries(client_fifo Qt6::Widgets)
target_link_libraries(host_mq Qt6::Widgets)
target_link_libraries(client_mq Qt6::Widgets)
target_link_libraries(host_seg Qt6::Widgets)
target_link_libraries(client_seg Qt6::Widgets)

# Путь для исполняемых файлов
set_target_properties(host_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(host_mq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client_mq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(host_seg PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(client_seg PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
