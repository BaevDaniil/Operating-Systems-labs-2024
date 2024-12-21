
cmake_minimum_required(VERSION 3.10)

project(library)

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
  host/host.cpp
  host/host_fifo.cpp
  connection/conn_fifo.cpp
)

set(CLIENT_FIFO_SRC_FILES
  client/client.cpp
  client/client_fifo.cpp
  connection/conn_fifo.cpp
)

# Host and Client source files for Queue
set(HOST_MQ_SRC_FILES
  host/host.cpp
  host/host_mq.cpp
  connection/conn_mq.cpp
)

set(CLIENT_MQ_SRC_FILES
  client/client.cpp
  client/client_mq.cpp
  connection/conn_mq.cpp
)

# Host and Client source files for Seg
set(HOST_SEG_SRC_FILES
  host/host.cpp
  host/host_seg.cpp
  connection/conn_seg.cpp
)

set(CLIENT_SEG_SRC_FILES
  client/client.cpp
  client/client_seg.cpp
  connection/conn_seg.cpp
)

set(SEMAPHORE
  semaphore/semaphore.cpp
)

set(GUI
  gui/client_gui.cpp
  gui/host_gui.cpp
)

# Ensure Qt's moc is run on the ChatWindow header
qt6_wrap_cpp(MOC_SOURCES gui/client_gui.h)
qt6_wrap_cpp(MOC_SOURCES gui/host_gui.h)
# qt6_wrap_cpp(MOC_SOURCES_CLIENT client/client.h)

# Add executables for FIFO communication
add_executable(host_fifo ${HOST_FIFO_SRC_FILES} ${SEMAPHORE} ${GUI} ${MOC_SOURCES})
add_executable(client_fifo ${CLIENT_FIFO_SRC_FILES} ${SEMAPHORE} ${GUI} ${MOC_SOURCES})


# Add executables for Queue communication
add_executable(host_mq ${HOST_MQ_SRC_FILES} ${SEMAPHORE} ${GUI} ${MOC_SOURCES})
add_executable(client_mq ${CLIENT_MQ_SRC_FILES} ${SEMAPHORE} ${GUI} ${MOC_SOURCES})

# Add executables for Queue communication
add_executable(host_seg ${HOST_SEG_SRC_FILES} ${SEMAPHORE} ${GUI} ${MOC_SOURCES})
add_executable(client_seg ${CLIENT_SEG_SRC_FILES} ${SEMAPHORE} ${GUI} ${MOC_SOURCES})

# Link Qt libraries to the GUI executable
target_link_libraries(host_fifo Qt6::Widgets)
target_link_libraries(client_fifo Qt6::Widgets)
target_link_libraries(host_mq Qt6::Widgets)
target_link_libraries(client_mq Qt6::Widgets)
target_link_libraries(host_seg Qt6::Widgets)
target_link_libraries(client_seg Qt6::Widgets)


# Set output directories for all executables
set_target_properties(host_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(client_fifo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(host_mq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(client_mq PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(host_seg PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(client_seg PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Custom target to build all
add_custom_target(all_targets DEPENDS host_fifo client_fifo host_mq client_mq host_seg client_seg)
