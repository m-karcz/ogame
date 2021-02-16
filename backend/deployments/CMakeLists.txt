add_subdirectory(deployment_common)
add_subdirectory(deployment_zmq_json_singlethread)

if(EMSCRIPTEN)
    add_subdirectory(deployment_wasm)
endif()