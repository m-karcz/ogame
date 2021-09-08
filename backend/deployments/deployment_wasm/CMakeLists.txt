add_executable(wasm_async WasmAsyncDeployment.cpp)
target_compile_features(wasm_async PRIVATE cxx_std_17)
target_link_options(wasm_async PRIVATE "--bind")
#target_link_options(wasm_async PRIVATE "-sEXPORTED_FUNCTIONS=[processRequest,_main,init]")
target_link_options(wasm_async PRIVATE "-sMODULARIZE=1")
target_link_options(wasm_async PRIVATE "-sASSERTIONS=2")
target_link_options(wasm_async PRIVATE "-sEXIT_RUNTIME=0")
target_link_options(wasm_async PRIVATE "-sEXTRA_EXPORTED_RUNTIME_METHODS=[writeAsciiToMemory,AsciiToString]")
target_link_options(wasm_async PRIVATE "-sEXPORT_NAME=makeBackend")
target_link_options(wasm_async PRIVATE --embed-file ${CONFIGURATION_PATH}@Configuration.json)
target_link_options(wasm_async PRIVATE "-o wasmtest.html")
target_link_options(wasm_async PRIVATE -lidbfs.js)
target_link_libraries(wasm_async DatabaseSQLite::DatabaseSQLite SerializationJson::SerializationJson RnD::RnD Configuration::Configuration)


set(RND_CALLER_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")


target_compile_definitions(wasm_async PRIVATE CONFIGURATION_FILE_DIR="${CMAKE_CURRENT_BINARY_DIR}/")

add_custom_command(OUTPUT rnd_caller.py
                   DEPENDS
                        rnd_caller_template.py
                   COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/rnd_caller_template.py ${CMAKE_CURRENT_BINARY_DIR}/rnd_caller.py)

add_custom_target(rnd_caller
                  DEPENDS rnd_caller.py)

add_executable(server ALIAS wasm_async)
                  