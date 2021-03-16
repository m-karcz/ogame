add_custom_command(OUTPUT frontend_npm.stamp
                   COMMAND npm install
                   COMMAND touch frontend_npm.stamp
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/frontend_npm.stamp
                   DEPENDS package.json
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(frontend_npm_install
                  DEPENDS frontend_npm.stamp)

if(EMSCRIPTEN)
    execute_process(
        COMMAND bash unhideWasm.sh 
        COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/src/WasmRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterConnectivity.ts
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(
        COMMAND cp ${GENERATED_PATH}                       ./src/ -r
        COMMAND cp ../router/src/IRouter.ts                ./src/
        COMMAND cp ../router/src/RouterMiddleware.ts       ./src/
        COMMAND cp ../router/src/OnPlanetRequestBuilder.ts ./src/
        COMMAND cp ../common/knowledge/Knowledge.json      ./src/
        COMMAND cp ../common/configuration/Configuration.json ./src/
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
    execute_process(
        COMMAND bash hideWasm.sh 
        COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/src/HttpRouterConnectivity.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/RouterConnectivity.ts
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(
        COMMAND cp ${GENERATED_PATH} ./src/ -r
        COMMAND cp ../common/knowledge/Knowledge.json ./src/
        COMMAND cp ../common/configuration/Configuration.json ./src/
        COMMAND rm ./src/IRouter.ts -f
        COMMAND rm ./src/RouterMiddleware.ts -f
        COMMAND rm ./src/OnPlanetRequestBuilder.ts -f
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    
endif()

file(GLOB frontend_src "src/*.ts" "src/*.tsx" "src/*.css" "src/*.scss")
list(FILTER frontend_src EXCLUDE REGEX ".*src/RouterConnectivity.ts$")

add_custom_command(OUTPUT frontend_cmd
                   DEPENDS ${frontend_src}
                   DEPENDS package.json
                   DEPENDS tsconfig.json
                   DEPENDS frontend_npm.stamp
                   COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMAND touch frontend_cmd)

add_custom_target(frontend
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