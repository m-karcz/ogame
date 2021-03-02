add_subdirectory(deployment_common)

if(EMSCRIPTEN)
    add_subdirectory(deployment_wasm)
else()
    add_subdirectory(deployment_zmq_json_singlethread)
endif()