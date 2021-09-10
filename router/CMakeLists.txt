add_custom_command(OUTPUT router_npm.stamp
        COMMAND npm install
        COMMAND touch router_npm.stamp
        COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/router_npm.stamp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(router_npm_install
                  DEPENDS router_npm.stamp)

set(ROUTER_EXECUTOR ${CMAKE_CURRENT_BINARY_DIR}/run_router)

get_target_property(FRONTEND_SOURCE_DIR frontend SOURCE_DIR)

file(GLOB router_src "src/*.ts")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/common_router_built.stamp
                   DEPENDS
                        router_npm.stamp
                        ${router_src}
                        frontend
                        server
                        package.json
                        tsconfig.json
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
                   COMMAND cp ${GENERATED_PATH}/generated/*.ts ${CMAKE_CURRENT_SOURCE_DIR}/src/generated
                   COMMAND npm run prestart
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/common_router_built.stamp)

if(EMSCRIPTEN)
add_custom_target(prepare_router
    BYPRODUCTS ${RUN_ROUTER}
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/common_router_built.stamp
    COMMAND printf \"\#!/bin/bash\\n\" > ${RUN_ROUTER}
    COMMAND echo "python3 -m http.server 8888 --directory ${FRONTEND_SOURCE_DIR}/build" >> ${RUN_ROUTER}
    COMMAND chmod +x ${RUN_ROUTER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
add_custom_target(prepare_router
    BYPRODUCTS ${RUN_ROUTER}
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/common_router_built.stamp
    COMMAND printf \"\#!/bin/bash\\n\" > ${RUN_ROUTER}
    COMMAND echo "node ${CMAKE_CURRENT_SOURCE_DIR}/build/src/router.js --binary \\\"$<TARGET_FILE:server>\\\"" >> ${RUN_ROUTER}
    COMMAND chmod +x ${RUN_ROUTER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_custom_target(run_router
    DEPENDS prepare_router
    COMMAND ${RUN_ROUTER})