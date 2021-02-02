cmake_minimum_required(VERSION 3.16)

add_library(SerializationJson Json.hpp toJson.hpp JsonSerializer.hpp JsonSerializer.cpp)
add_library(SerializationJson::SerializationJson ALIAS SerializationJson)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET SerializationJson PROPERTY CXX_STANDARD 17)
target_link_libraries(SerializationJson nlohmann_json::nlohmann_json SerializationInterface::SerializationInterface Utils::Utils)
target_include_directories(SerializationJson PUBLIC .)

