add_library(connections STATIC
    connection.hpp
    conn_mq.hpp
    conn_mq.cpp
    conn_fifo.hpp
    conn_fifo.cpp
    all_connections.hpp
)
set_target_properties(connections PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(connections PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})