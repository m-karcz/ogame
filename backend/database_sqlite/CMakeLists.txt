cmake_minimum_required(VERSION 3.16)

add_library(DatabaseSQLite StorageDb.hpp StorageDb.cpp NamedColumn.hpp PlanetWrapper.hpp ColumnOptions.hpp Constraints.hpp)
add_library(DatabaseSQLite::DatabaseSQLite ALIAS DatabaseSQLite)

add_subdirectory(sqlite_orm)

#target_compile_features(Utils PRIVATE cxx_std_17)
#set_property()
set (CMAKE_CXX_STANDARD 17)
set_property(TARGET DatabaseSQLite PROPERTY CXX_STANDARD 17)
target_include_directories(DatabaseSQLite PUBLIC .)
target_link_libraries(DatabaseSQLite DatabaseInterface::DatabaseInterface SQLiteOrm::SQLiteOrm Utils::Utils)

