add_library(host STATIC
    client_info.hpp
    host.hpp
)
set_target_properties(host PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(host PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})