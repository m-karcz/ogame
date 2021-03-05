execute_process(COMMAND npm install
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(ROUTER_EXECUTOR ${CMAKE_CURRENT_BINARY_DIR}/run_router)

get_target_property(FRONTEND_SOURCE_DIR frontend SOURCE_DIR)

add_custom_command(OUTPUT common_router_built.stamp
                   DEPENDS
                        frontend
                        server
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMAND cp ${GENERATED_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/ -r
                   COMMAND npm run prestart
                   COMMAND printf \"\#!/bin/bash\\n\" > ${RUN_ROUTER}
                   COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/common_router_built.stamp)

if(EMSCRIPTEN)
add_custom_target(prepare_router
    BYPRODUCTS ${RUN_ROUTER}
    DEPENDS
        common_router_built.stamp
    COMMAND echo "python3 -m http.server 8888 --directory ${FRONTEND_SOURCE_DIR}/build" >> ${RUN_ROUTER}
    COMMAND chmod +x ${RUN_ROUTER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
else()
add_custom_target(prepare_router
    BYPRODUCTS ${RUN_ROUTER}
    DEPENDS
        common_router_built.stamp
    COMMAND echo "node ${CMAKE_CURRENT_SOURCE_DIR}/build/src/router.js --binary \\\"$<TARGET_FILE:server>\\\"" >> ${RUN_ROUTER}
    COMMAND chmod +x ${RUN_ROUTER}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_custom_target(run_router
    DEPENDS prepare_router
    COMMAND ${RUN_ROUTER})