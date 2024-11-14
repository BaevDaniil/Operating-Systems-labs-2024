add_library(connections STATIC
    conn_mq.cpp
    conn_fifo.cpp
)
target_include_directories(connections PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})