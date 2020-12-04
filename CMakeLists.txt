cmake_minimum_required(VERSION 3.16)

project(ogame)

include(CMakeLists.txt.externals)
find_package(Boost 1.71 COMPONENTS program_options REQUIRED)

#add_subdirectory(external_libraries/boost)

add_subdirectory(sqlite)
add_subdirectory(sqlite_orm)
add_subdirectory(utils)
add_subdirectory(data)
add_subdirectory(tests)
add_subdirectory(logic)
add_subdirectory(json)
add_subdirectory(database_interface)
add_subdirectory(database_sqlite)
#add_subdirectory(serialization)
add_subdirectory(rnd)


add_executable(ogame main.cpp)
add_executable(dummy_io_ogame dummy_io.cpp)
add_executable(server server2.cpp)
add_subdirectory(robot_tests)
target_compile_features(ogame PRIVATE cxx_std_17)
target_compile_features(server PRIVATE cxx_std_17)
target_link_libraries(ogame SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite)
target_link_libraries(server SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite Json::Json ${Boost_LIBRARIES} RnD::RnD)
