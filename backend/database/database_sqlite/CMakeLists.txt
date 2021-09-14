cmake_minimum_required(VERSION 3.16)

add_subdirectory(sqlite)

add_library(DatabaseSQLite MakeTable.hpp StorageDb.hpp StorageDb.cpp)
add_library(DatabaseSQLite::DatabaseSQLite ALIAS DatabaseSQLite)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET DatabaseSQLite PROPERTY CXX_STANDARD 17)
target_include_directories(DatabaseSQLite PUBLIC .)
target_link_libraries(DatabaseSQLite DatabaseInterface::DatabaseInterface SQLite3::SQLite3)