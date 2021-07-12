cmake_minimum_required(VERSION 3.16)

add_library(Messaging Event.hpp)
add_library(Messaging::Messaging ALIAS Messaging)

#find_package(Boost REQUIRED)
#target_compile_features(Utils PRIVATE cxx_std_17)
#set_property()
set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Messaging PROPERTY CXX_STANDARD 17)
target_link_libraries(Messaging spdlog::spdlog range-v3::range-v3 SerializationInterface::SerializationInterface)
target_include_directories(Messaging PUBLIC .)

