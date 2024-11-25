cmake_minimum_required(VERSION 3.10)

project(LocalChatApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Set the path for Qt6
set(CMAKE_PREFIX_PATH "/home/barulina/Qt/6.8.0/gcc_64/lib/cmake")

# Include directories
include_directories(include)

# Find Qt6 package
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Host and Client source files for FIFO
set(HOST_FIFO_SRC_FILES
  host/host_fifo.cpp
  connection/conn_fifo.cpp
)

set(CLIENT_FIFO_SRC_FILES
  client/client_fifo.cpp
  connection/conn_fifo.cpp
)

# # Host and Client source files for Queue
# set(HOST_QUEUE_SRC_FILES
#   host/host_queue.cpp
#   connection/conn_queue.cpp
# )

# set(CLIENT_QUEUE_SRC_FILES
#   client/client_queue.cpp
#   connection/conn_queue.cpp
# )

# # Host and Client source files for Socket
# set(HOST_SOCKET_SRC_FILES
#   client/host_socket.cpp
#   connection/conn_socket.cpp
# )

# set(CLIENT_SOCKET_SRC_FILES
#   client/client_socket.cpp
#   connection/conn_socket.cpp
# )

set(GUI
  gui/client_gui.cpp
  gui/host_gui.cpp
)

# Ensure Qt's moc is run on the ChatWindow header
qt6_wrap_cpp(MOC_SOURCES gui/client_gui.h)
qt6_wrap_cpp(MOC_SOURCES gui/host_gui.h)

# Add executables for FIFO communication
add_executable(host_fifo ${HOST_FIFO_SRC_FILES} ${GUI} ${MOC_SOURCES})
add_executable(client_fifo ${CLIENT_FIFO_SRC_FILES} ${GUI} ${MOC_SOURCES})

# # Add executables for Queue communication
# add_executable(host_queue ${HOST_QUEUE_SRC_FILES} ${GUI} ${MOC_SOURCES})
# add_executable(client_queue ${CLIENT_QUEUE_SRC_FILES} ${GUI} ${MOC_SOURCES})

# # Add executables for Socket communication
# add_executable(host_socket ${HOST_SOCKET_SRC_FILES} ${GUI} ${MOC_SOURCES})
# add_executable(client_socket ${CLIENT_SOCKET_SRC_FILES} ${GUI} ${MOC_SOURCES})

# Link Qt libraries to the GUI executable
target_link_libraries(host_fifo Qt6::Widgets)
target_link_libraries(client_fifo Qt6::Widgets)
# target_link_libraries(host_queue Qt6::Widgets)
# target_link_libraries(client_queue Qt6::Widgets)
# target_link_libraries(host_socket Qt6::Widgets)
# target_link_libraries(client_socket Qt6::Widgets)

# Set output directories for all executables
set_target_properties(host_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(client_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# set_target_properties(host_queue PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set_target_properties(client_queue PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set_target_properties(host_socket PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set_target_properties(client_socket PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Custom target to build all
# add_custom_target(all_targets DEPENDS host_fifo client_fifo host_queue client_queue host_socket client_socket)

add_custom_target(all_targets DEPENDS host_fifo client_fifo)
