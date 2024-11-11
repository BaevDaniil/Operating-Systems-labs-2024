add_library(daemon STATIC
    daemon.hpp
    daemon.cpp
)
set_target_properties(daemon PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(daemon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})