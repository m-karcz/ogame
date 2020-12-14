cmake_minimum_required(VERSION 3.16)

add_library(Json dummy.cpp Json.hpp toJson.hpp)
add_library(Json::Json ALIAS Json)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Json PROPERTY CXX_STANDARD 17)
target_link_libraries(Json nlohmann_json::nlohmann_json)
target_include_directories(Json PUBLIC .)

