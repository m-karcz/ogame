cmake_minimum_required(VERSION 3.16)

add_library(ResourceOwner
            ResourceOwner.hpp
            ResourceOwner.cpp
            ResourceOwnerApp.cpp)

add_library(ResourceOwner::ResourceOwner ALIAS ResourceOwner)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET ResourceOwner PROPERTY CXX_STANDARD 17)
target_include_directories(ResourceOwner PUBLIC .)
target_link_libraries(ResourceOwner Data::Data range-v3::range-v3 Messaging::Messaging)

