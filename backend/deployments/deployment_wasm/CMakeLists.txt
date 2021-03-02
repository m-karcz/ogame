add_executable(wasm_sync WasmSyncDeployment.cpp)
target_compile_features(wasm_sync PRIVATE cxx_std_17)
target_link_options(wasm_sync PRIVATE "--bind")
#target_link_options(wasm_sync PRIVATE "-sEXPORTED_FUNCTIONS=[processRequest,_main]")
target_link_options(wasm_sync PRIVATE "-sMODULARIZE=1")
target_link_options(wasm_sync PRIVATE "-sASSERTIONS=2")
target_link_options(wasm_sync PRIVATE "-sEXTRA_EXPORTED_RUNTIME_METHODS=[writeAsciiToMemory,AsciiToString]")
target_link_options(wasm_sync PRIVATE "-sEXPORT_NAME=makeBackend")
target_link_options(wasm_sync PRIVATE "-o wasmtest.html")
#target_link_options(wasm_sync PRIVATE "-sWASM_ASYNC_COMPILATION=0")
target_link_libraries(wasm_sync DatabaseInMemory::DatabaseInMemory SerializationJson::SerializationJson RnD::RnD DeploymentCommon::DeploymentCommon)


add_custom_command(OUTPUT time_forwarder.py
                   DEPENDS
                        time_forwarder_template.py
                   COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/time_forwarder_template.py ${CMAKE_CURRENT_BINARY_DIR}/time_forwarder.py)

add_custom_target(time_forwarder
                  DEPENDS time_forwarder.py)
                  
set(TIME_FORWARDER_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")