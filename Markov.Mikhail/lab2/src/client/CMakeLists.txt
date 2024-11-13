add_library(client STATIC
    client.hpp
    client.cpp
)
set_target_properties(client PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})