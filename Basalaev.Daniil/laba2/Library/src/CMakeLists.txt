
add_library(LibraryLib
    src/Client/Client.cpp
    src/Client/ClientWindow.cpp
    src/Common/Http.cpp
    src/Common/Logger.cpp
    src/Common/SemaphoreLocal.cpp
    src/conn/conn_fifo.cpp
    src/conn/conn_sock.cpp
    src/Host/Host.cpp
    src/Host/HostWindow.cpp
)

target_link_libraries(LibraryLib Qt5::Widgets)

target_include_directories(LibraryLib PUBLIC
    src/Client
    src/Common
    src/conn
    src/Host
)

target_include_directories(LibraryLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
