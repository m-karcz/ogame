cmake_minimum_required(VERSION 3.16)

add_library(DatabaseInMemory StorageDb.hpp StorageDb.cpp StorageDbFactory.hpp)
add_library(DatabaseInMemory::DatabaseInMemory ALIAS DatabaseInMemory)

#target_compile_features(Utils PRIVATE cxx_std_17)
#set_property()
set (CMAKE_CXX_STANDARD 17)
set_property(TARGET DatabaseInMemory PROPERTY CXX_STANDARD 17)
target_include_directories(DatabaseInMemory PUBLIC .)
target_link_libraries(DatabaseInMemory DatabaseInterface::DatabaseInterface Utils::Utils)

