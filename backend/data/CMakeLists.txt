cmake_minimum_required(VERSION 3.16)

add_library(Data  Ships.hpp Timestamp.hpp)
add_library(Data::Data ALIAS Data)


set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Data PROPERTY CXX_STANDARD 17)

target_include_directories(Data PUBLIC .)
target_link_libraries(Data Utils::Utils GenHeaders::GenHeaders)
set_target_properties(Data PROPERTIES LINKER_LANGUAGE CXX)

