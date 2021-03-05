add_custom_command(OUTPUT frontend_npm.stamp
                   COMMAND npm install
                   COMMAND touch frontend_npm.stamp
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/frontend_npm.stamp
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(frontend_npm_install
                  DEPENDS frontend_npm_install_command)

if(EMSCRIPTEN)
    execute_process(
        COMMAND cp ${GENERATED_PATH}                       ./src/ -r
        COMMAND cp ../router/src/IRouter.ts                ./src/
        COMMAND cp ../router/src/RouterMiddleware.ts       ./src/
        COMMAND cp ../router/src/OnPlanetRequestBuilder.ts ./src/
        COMMAND cp ../common/knowledge/Knowledge.json      ./src/
        COMMAND mv -f ./src/WasmRouterConnectivity.ts.unused ./src/WasmRouterConnectivity.ts || echo " "
        COMMAND ln -sf ./WasmRouterConnectivity.ts ./src/RouterConnectivity.ts
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    
else()
    execute_process(
        COMMAND cp ${GENERATED_PATH} ./src/ -r
        COMMAND cp ../common/knowledge/Knowledge.json ./src/
        COMMAND rm ./src/IRouter.ts -f
        COMMAND rm ./src/RouterMiddleware.ts -f
        COMMAND rm ./src/OnPlanetRequestBuilder.ts -f
        COMMAND mv -f ./src/WasmRouterConnectivity.ts ./src/WasmRouterConnectivity.ts.unused || echo " "
        COMMAND ln -sf ./HttpRouterConnectivity.ts ./src/RouterConnectivity.ts
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    
endif()

file(GLOB frontend_src "src/*.ts" "src/*.tsx" "src/*.css")
list(FILTER frontend_src EXCLUDE REGEX ".*src/RouterConnectivity.ts$")

add_custom_command(OUTPUT frontend_cmd
                   DEPENDS ${frontend_src}
                   COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMAND touch frontend_cmd)

add_custom_target(frontend
                  DEPENDS frontend_npm.stamp
                  DEPENDS frontend_cmd)

if(EMSCRIPTEN)
    add_custom_command(
        TARGET frontend
        COMMAND cp $<TARGET_FILE_DIR:wasm_sync>/wasm_sync* ${CMAKE_CURRENT_SOURCE_DIR}/build/)
else()
    add_custom_command(
        TARGET frontend
        COMMAND touch ${CMAKE_CURRENT_SOURCE_DIR}/build/wasm_sync.js)
endif()