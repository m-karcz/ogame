cmake_minimum_required(VERSION 3.16)

add_library(SerializationInterface ISerializer.hpp)
add_library(SerializationInterface::SerializationInterface ALIAS SerializationInterface)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET SerializationInterface PROPERTY CXX_STANDARD 17)
target_include_directories(SerializationInterface PUBLIC .)
target_link_libraries(SerializationInterface Data::Data)
set_target_properties(SerializationInterface PROPERTIES LINKER_LANGUAGE CXX)

