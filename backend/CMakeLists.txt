
include(CMakeLists.txt.externals)
find_package(Boost 1.65 COMPONENTS program_options OPTIONAL)

add_subdirectory(utils)
add_subdirectory(data)
add_subdirectory(serialization)
add_subdirectory(tests)
add_subdirectory(logic)
add_subdirectory(database)
add_subdirectory(rnd)
add_subdirectory(deployments)

add_executable(ogame main.cpp)
#add_executable(server server2.cpp)
#add_executable(server zmqMain.cpp)
#target_compile_features(ogame PRIVATE cxx_std_17)
#target_compile_features(server PRIVATE cxx_std_17)

add_executable(server ALIAS zmq_json_1thread)

add_executable(server.js wasmMain.cpp)
target_compile_features(server.js PRIVATE cxx_std_17)
target_link_libraries(server.js SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite SerializationJson::SerializationJson RnD::RnD)

#target_compile_features(zmqserver PRIVATE cxx_std_17)
#target_link_libraries(zmqserver SQLiteOrm::SQLiteOrm Logic::Logic DatabaseSQLite::DatabaseSQLite SerializationJson::SerializationJson ${Boost_LIBRARIES} RnD::RnD cppzmq)



