add_library(includes STATIC
    includes.hpp
)
set_target_properties(includes PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(includes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})