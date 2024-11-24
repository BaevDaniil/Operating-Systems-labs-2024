cmake_minimum_required(VERSION 3.5)

project(Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Client main.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "KPDA")
    set(INSTALL_DESTDIR "/tmp")

    install(TARGETS Client
        RUNTIME DESTINATION "${INSTALL_DESTDIR}"
        BUNDLE DESTINATION "${INSTALL_DESTDIR}"
        LIBRARY DESTINATION "${INSTALL_DESTDIR}"
    )
endif()
