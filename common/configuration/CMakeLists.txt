set(CONFIGURATION_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Configuration.json CACHE INTERNAL "")

if(DEMO)
    execute_process(
        COMMAND ln -sf DemoConfiguration.json ${CONFIGURATION_PATH}
    )
else()
    execute_process(
        COMMAND ln -sf RndConfiguration.json ${CONFIGURATION_PATH}
    )
endif()