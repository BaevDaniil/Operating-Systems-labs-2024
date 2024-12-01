
add_library(LibraryLib
    Client/Client.cpp
    Client/ClientWindow.cpp
    Common/Http.cpp
    Common/LibraryWindowImpl.cpp
    Common/Logger.cpp
    Common/SemaphoreLocal.cpp
    Common/Utils.cpp
    Conn/conn_fifo.cpp
    Conn/conn_sock.cpp
    Host/Host.cpp
    Host/HostWindow.cpp
)

target_link_libraries(LibraryLib Qt5::Widgets)

# target_include_directories(LibraryLib PUBLIC
#     src/Client
#     src/Common
#     src/conn
#     src/Host
# )

target_include_directories(LibraryLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
