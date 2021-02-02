cmake_minimum_required(VERSION 3.16)

add_library(RnD RnDService.hpp RnDService.cpp)
add_library(RnD::RnD ALIAS RnD)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET RnD PROPERTY CXX_STANDARD 17)
target_include_directories(RnD PUBLIC .)
target_link_libraries(RnD Data::Data DatabaseInterface::DatabaseInterface Logic::Logic)


add_executable(RnDZmqTimeForwarderApp RnDZmqTimeForwarderApp.cpp)
target_link_libraries(RnDZmqTimeForwarderApp SerializationJson::SerializationJson cppzmq Data::Data)