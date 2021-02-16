execute_process(COMMAND npm install
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(EMSCRIPTEN)
add_custom_target(frontend
    DEPENDS wasm_sync
    COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterConnectivity.ts
    COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND cp $<TARGET_FILE_DIR:wasm_sync>/wasm_sync* ${CMAKE_CURRENT_SOURCE_DIR}/build/)
else()
add_custom_target(frontend
    DEPENDS
    COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/src/HttpRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterConnectivity.ts
    COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR})
endif()

