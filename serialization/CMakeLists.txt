cmake_minimum_required(VERSION 3.16)

add_library(Serialization dummy.cpp JsonDeserialization.cpp JsonDeserialization.hpp)
add_library(Serialization::Serialization ALIAS Serialization)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Serialization PROPERTY CXX_STANDARD 17)
target_link_libraries(Serialization Json::Json Data::Data)
target_include_directories(Serialization PUBLIC .)

