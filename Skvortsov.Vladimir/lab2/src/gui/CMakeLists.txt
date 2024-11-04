# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Specify the project name
project(ChatGui)

# Ensure C++17 standard is used
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 Widgets
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# UI files and source files
set(GUI_FILES
  ChatWindow.cpp
  main.cpp
)

# Ensure Qt's moc is run on the ChatWindow header
qt6_wrap_cpp(MOC_SOURCES ChatWindow.hpp)

# Add executable for GUI
add_executable(chat_gui ${GUI_FILES} ${MOC_SOURCES})

# Link Qt libraries
target_link_libraries(chat_gui Qt6::Widgets)

# Set output directory for chat_gui
set_target_properties(chat_gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
