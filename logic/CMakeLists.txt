cmake_minimum_required(VERSION 3.16)

add_library(Logic Service.hpp Service.cpp)
add_library(Logic::Logic ALIAS Logic)

#target_compile_features(Utils PRIVATE cxx_std_17)
#set_property()
set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Logic PROPERTY CXX_STANDARD 17)
target_include_directories(Logic PUBLIC .)
target_link_libraries(Logic Data::Data Json::Json DatabaseInterface::DatabaseInterface)

