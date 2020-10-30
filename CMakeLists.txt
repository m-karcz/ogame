cmake_minimum_required(VERSION 3.16)

project(ogame)

set(CMAKE_GENERATOR "Ninja")

add_subdirectory(sqlite)
add_subdirectory(sqlite_orm)

add_executable(ogame main.cpp)
target_link_libraries(ogame SQLiteOrm)
