add_executable(zmq_json_1thread ZmqJson1Thread.cpp)
target_compile_features(zmq_json_1thread PRIVATE cxx_std_17)
target_link_libraries(zmq_json_1thread DatabaseSQLite::DatabaseSQLite SerializationJson::SerializationJson RnD::RnD cppzmq DeploymentCommon::DeploymentCommon Configuration::Configuration)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Configuration.json
                   COMMAND ln -sf ${CONFIGURATION_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Configuration.json 
                   DEPENDS ${CONFIGURATION_PATH})

target_compile_definitions(zmq_json_1thread PRIVATE CONFIGURATION_FILE_DIR="${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_target(put_configuration
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Configuration.json)

add_dependencies(zmq_json_1thread put_configuration)

add_executable(RnDZmqTimeForwarderApp RnDZmqTimeForwarderApp.cpp)
target_compile_features(RnDZmqTimeForwarderApp PRIVATE cxx_std_17)
target_link_libraries(RnDZmqTimeForwarderApp SerializationJson::SerializationJson cppzmq Data::Data)


#add_custom_command(OUTPUT time_forwarder.py
#                   DEPENDS
#                        RnDZmqTimeForwarderApp
#                        time_forwarder_template.py
#                   COMMAND sed \"s@TIME_FORWARDER_APP_PATH@$<TARGET_FILE:RnDZmqTimeForwarderApp>@g\" ${CMAKE_CURRENT_SOURCE_DIR}/time_forwarder_template.py > ${CMAKE_CURRENT_BINARY_DIR}/time_forwarder.py)
#
#add_custom_target(time_forwarder
#                  DEPENDS time_forwarder.py)
#                  
#set(TIME_FORWARDER_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

set(RND_CALLER_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

add_custom_command(OUTPUT rnd_caller.py
                   DEPENDS
                        RnDZmqTimeForwarderApp
                        rnd_caller_template.py
                   COMMAND sed \"s@RND_CALLER_PATH@$<TARGET_FILE:RnDZmqTimeForwarderApp>@g\" ${CMAKE_CURRENT_SOURCE_DIR}/rnd_caller_template.py > ${CMAKE_CURRENT_BINARY_DIR}/rnd_caller.py)

add_custom_target(rnd_caller
                  DEPENDS rnd_caller.py)
                  

add_executable(server ALIAS zmq_json_1thread)