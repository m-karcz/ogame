cmake_minimum_required(VERSION 3.16)


add_library(SQLite3 sqlite3.h sqlite3.c sqlite3ext.h)
add_library(SQLite3::SQLite3 ALIAS SQLite3)

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
add_definitions(-DSQLITE_ENABLE_JSON1)
add_definitions(-DSQLITE_ENABLE_RBU)
add_definitions(-DSQLITE_ENABLE_STAT4)

target_include_directories(SQLite3 PUBLIC .)

