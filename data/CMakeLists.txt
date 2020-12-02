cmake_minimum_required(VERSION 3.16)

add_library(Data  Buildings.hpp Researchs.hpp Ships.hpp Storage.hpp PlayerId.hpp Timestamp.hpp dummy.cpp RegisterRequest.hpp)
add_library(Data::Data ALIAS Data)


set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Data PROPERTY CXX_STANDARD 17)

target_include_directories(Data PUBLIC .)
target_link_libraries(Data Utils::Utils)


