add_executable(wasm_sync WasmSyncDeployment.cpp)
target_compile_features(wasm_sync PRIVATE cxx_std_17)
target_link_options(wasm_sync PRIVATE "--bind")
#target_link_options(wasm_sync PRIVATE "-sEXPORTED_FUNCTIONS=[processRequest,_main,init]")
target_link_options(wasm_sync PRIVATE "-sMODULARIZE=1")
target_link_options(wasm_sync PRIVATE "-sASSERTIONS=2")
target_link_options(wasm_sync PRIVATE "-sEXTRA_EXPORTED_RUNTIME_METHODS=[writeAsciiToMemory,AsciiToString]")
target_link_options(wasm_sync PRIVATE "-sEXPORT_NAME=makeBackend")
target_link_options(wasm_sync PRIVATE --embed-file ${CONFIGURATION_PATH}@Configuration.json)
target_link_options(wasm_sync PRIVATE "-o wasmtest.html")
target_link_options(wasm_sync PRIVATE -lidbfs.js)
#target_link_options(wasm_sync PRIVATE "-sWASM_ASYNC_COMPILATION=0")
target_link_libraries(wasm_sync DatabaseSQLite::DatabaseSQLite SerializationJson::SerializationJson RnD::RnD DeploymentCommon::DeploymentCommon Configuration::Configuration)


set(RND_CALLER_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")


add_custom_command(OUTPUT rnd_caller.py
                   DEPENDS
                        rnd_caller_template.py
                   COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/rnd_caller_template.py ${CMAKE_CURRENT_BINARY_DIR}/rnd_caller.py)

add_custom_target(rnd_caller
                  DEPENDS rnd_caller.py)

add_executable(server ALIAS wasm_sync)
                  