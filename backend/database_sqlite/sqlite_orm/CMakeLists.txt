cmake_minimum_required(VERSION 3.16)

add_library(SQLiteOrm sqlite_orm.hpp dummy.cpp)
add_library(SQLiteOrm::SQLiteOrm ALIAS SQLiteOrm)

add_subdirectory(sqlite)

target_include_directories(SQLiteOrm PUBLIC .)
target_link_libraries(SQLiteOrm SQLite3::SQLite3)

