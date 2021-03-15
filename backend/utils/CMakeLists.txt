cmake_minimum_required(VERSION 3.16)

add_library(Utils BigNum.hpp dummy.cpp Logger.cpp)
add_library(Utils::Utils ALIAS Utils)

#find_package(Boost REQUIRED)
#target_compile_features(Utils PRIVATE cxx_std_17)
#set_property()
set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Utils PROPERTY CXX_STANDARD 17)
target_link_libraries(Utils spdlog::spdlog range-v3::range-v3)
target_include_directories(Utils PUBLIC .)

