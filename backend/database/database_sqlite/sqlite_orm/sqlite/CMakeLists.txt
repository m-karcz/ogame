cmake_minimum_required(VERSION 3.16)

FetchContent_Declare(
        sqlite_amalgamation
        URL    https://www.sqlite.org/2020/sqlite-amalgamation-3340000.zip)

FetchContent_Populate(sqlite_amalgamation)

add_library(SQLite3 ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c
                    ${sqlite_amalgamation_SOURCE_DIR}/sqlite3.h
                    ${sqlite_amalgamation_SOURCE_DIR}/sqlite3ext.h)
add_library(SQLite3::SQLite3 ALIAS SQLite3)

find_package(Threads REQUIRED)

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
add_definitions(-DSQLITE_ENABLE_JSON1)
add_definitions(-DSQLITE_ENABLE_RBU)
add_definitions(-DSQLITE_ENABLE_STAT4)

target_include_directories(SQLite3 PUBLIC ${sqlite_amalgamation_SOURCE_DIR})
target_link_libraries(SQLite3 Threads::Threads ${CMAKE_DL_LIBS})

