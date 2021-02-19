execute_process(COMMAND npm install
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(ROUTER_EXECUTOR ${CMAKE_CURRENT_BINARY_DIR}/run_router)

if(EMSCRIPTEN)
get_target_property(FRONTEND_SOURCE_DIR frontend SOURCE_DIR)

add_custom_target(prepare_router
    BYPRODUCTS ${RUN_ROUTER}
    DEPENDS
        wasm_sync
        frontend
    COMMAND cp ${GENERATED_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/generated -r
    COMMAND npm run prestart
    COMMAND printf \"\#!/bin/bash\\n\" > ${RUN_ROUTER}
    COMMAND echo "python3 -m http.server 8888 --directory ${FRONTEND_SOURCE_DIR}/build" >> ${RUN_ROUTER}
    COMMAND chmod +x ${RUN_ROUTER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(run_router
    DEPENDS prepare_router
    COMMAND ${RUN_ROUTER})
else()
add_custom_target(prepare_router
    BYPRODUCTS ${RUN_ROUTER}
    DEPENDS
        #${CMAKE_CURRENT_SOURCE_DIR}/node_modules

        server
        frontend
    COMMAND cp ${GENERATED_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/ -r
    COMMAND npm run prestart
    COMMAND printf \"\#!/bin/bash\\n\" > ${RUN_ROUTER}
    COMMAND echo "node ${CMAKE_CURRENT_SOURCE_DIR}/build/src/router.js --binary \\\"$<TARGET_FILE:server>\\\"" >> ${RUN_ROUTER}
    COMMAND chmod +x ${RUN_ROUTER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(run_router
    DEPENDS prepare_router
    COMMAND ${RUN_ROUTER})
endif()
