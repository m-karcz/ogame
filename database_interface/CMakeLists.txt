cmake_minimum_required(VERSION 3.16)

add_library(DatabaseInterface dummy.cpp)
add_library(DatabaseInterface::DatabaseInterface ALIAS DatabaseInterface)

#target_compile_features(Utils PRIVATE cxx_std_17)
#set_property()
set (CMAKE_CXX_STANDARD 17)
set_property(TARGET DatabaseInterface PROPERTY CXX_STANDARD 17)
target_include_directories(DatabaseInterface PUBLIC .)
target_link_libraries(DatabaseInterface Data::Data)

