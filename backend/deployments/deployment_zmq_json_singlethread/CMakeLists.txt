add_executable(zmq_json_1thread ZmqJson1Thread.cpp)
target_compile_features(zmq_json_1thread PRIVATE cxx_std_17)
target_link_libraries(zmq_json_1thread DatabaseInMemory::DatabaseInMemory SerializationJson::SerializationJson RnD::RnD cppzmq DeploymentCommon::DeploymentCommon)


add_executable(RnDZmqTimeForwarderApp RnDZmqTimeForwarderApp.cpp)
target_compile_features(RnDZmqTimeForwarderApp PRIVATE cxx_std_17)
target_link_libraries(RnDZmqTimeForwarderApp SerializationJson::SerializationJson cppzmq Data::Data)


add_custom_command(OUTPUT time_forwarder.py
                   DEPENDS
                        RnDZmqTimeForwarderApp
                        time_forwarder_template.py
                   COMMAND sed \"s@TIME_FORWARDER_APP_PATH@$<TARGET_FILE:RnDZmqTimeForwarderApp>@g\" ${CMAKE_CURRENT_SOURCE_DIR}/time_forwarder_template.py > ${CMAKE_CURRENT_BINARY_DIR}/time_forwarder.py)

add_custom_target(time_forwarder
                  DEPENDS time_forwarder.py)
                  
set(TIME_FORWARDER_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

add_executable(server ALIAS zmq_json_1thread)