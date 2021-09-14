cmake_minimum_required(VERSION 3.16)

add_library(RnD RnDTime.hpp)
add_library(RnD::RnD ALIAS RnD)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET RnD PROPERTY CXX_STANDARD 17)
target_include_directories(RnD PUBLIC .)
target_link_libraries(RnD Data::Data DatabaseInterface::DatabaseInterface Logic::Logic ResourceOwner::ResourceOwner)




