add_library(connections STATIC
    connection.hpp
    conn_message_queue.hpp
    conn_message_queue.cpp
    conn_named_chanel.hpp
    conn_named_chanel.cpp
    all_connections.hpp
)
set_target_properties(connections PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(connections PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})