cmake_minimum_required(VERSION 3.14)

project(WolfAndGoats VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Werror)

find_library (LIBRT rt)
find_package (Threads)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

file(GLOB CONNS connections/conn_*.h)
file(GLOB CLIENT client/*.cpp client/*.h)
file(GLOB GUI gui/*.ui gui/*.h gui/*.cpp)
file(GLOB HOST host/*.cpp host/*.h)

message("--Gernerate ui_ ${GUI_UI}")
foreach(PATH_TO_CONN ${CONNS})
    get_filename_component(FILE_NAME ${PATH_TO_CONN} NAME_WLE )
    string(REGEX REPLACE "conn_" "" CONN_NAME ${FILE_NAME})
    
    add_executable(host_${CONN_NAME} ${GUI} ${GUI_UI} ${GUI_SRC} ${HOST} connections/connection.h 
    connections/conn_${CONN_NAME}.h connections/conn_${CONN_NAME}.cpp)
    target_link_libraries(host_${CONN_NAME} PUBLIC pthread rt Qt6::Widgets)

    add_executable(client_${CONN_NAME} ${CLIENT} connections/connection.h
        connections/conn_${CONN_NAME}.cpp connections/conn_${CONN_NAME}.h)
    target_link_libraries(client_${CONN_NAME} PUBLIC pthread rt)
endforeach()