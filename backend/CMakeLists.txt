
include(CMakeLists.txt.externals)
find_package(Boost 1.65 COMPONENTS program_options OPTIONAL)

add_subdirectory(utils)
add_subdirectory(data)
add_subdirectory(tests)
add_subdirectory(logic)
add_subdirectory(json)
add_subdirectory(database_interface)
add_subdirectory(database_sqlite)
add_subdirectory(rnd)

add_executable(ogame main.cpp)
add_executable(server server2.cpp)
target_compile_features(ogame PRIVATE cxx_std_17)
target_compile_features(server PRIVATE cxx_std_17)
target_link_libraries(ogame SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite)
target_link_libraries(server SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite Json::Json ${Boost_LIBRARIES} RnD::RnD)

add_executable(server.js wasmMain.cpp)
target_compile_features(server.js PRIVATE cxx_std_17)
target_link_libraries(server.js SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite Json::Json RnD::RnD)


