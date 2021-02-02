cmake_minimum_required(VERSION 3.16)

FetchContent_Declare(
        sqlite_orm
        GIT_REPOSITORY https://github.com/fnc12/sqlite_orm
        GIT_TAG v1.5)

FetchContent_Populate(sqlite_orm)

add_library(SQLiteOrm ${sqlite_orm_SOURCE_DIR}/include/sqlite_orm/sqlite_orm.h)
add_library(SQLiteOrm::SQLiteOrm ALIAS SQLiteOrm)

add_subdirectory(sqlite)

target_include_directories(SQLiteOrm PUBLIC ${sqlite_orm_SOURCE_DIR}/include/sqlite_orm)
target_link_libraries(SQLiteOrm SQLite3::SQLite3)
set_target_properties(SQLiteOrm PROPERTIES LINKER_LANGUAGE CXX)

