execute_process(COMMAND npm install
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


if(EMSCRIPTEN)
add_custom_target(frontend
    DEPENDS wasm_sync
    COMMAND cp ${GENERATED_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/ -r
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../router/src/IRouter.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../router/src/RouterMiddleware.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/../router/src/OnPlanetRequestBuilder.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMAND mv -f ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts.unused ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts || echo " "
    COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterConnectivity.ts
    COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND cp $<TARGET_FILE_DIR:wasm_sync>/wasm_sync* ${CMAKE_CURRENT_SOURCE_DIR}/build/)
else()
add_custom_target(frontend
    DEPENDS
    COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/src/IRouter.ts -f
    COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterMiddleware.ts -f
    COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/src/OnPlanetRequestBuilder.ts -f
    COMMAND cp ${GENERATED_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/ -r
    COMMAND mv -f ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts.unused || echo " "
    COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/src/HttpRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterConnectivity.ts
    COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND touch ${CMAKE_CURRENT_SOURCE_DIR}/wasm_sync.js)
endif()

