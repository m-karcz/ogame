add_custom_command(
    OUTPUT test_configuration.json
    DEPENDS
        test_configuration_template.json
        time_forwarder
    COMMAND
        sed \"s@TIME_FORWARDER_PATH@${TIME_FORWARDER_PATH}@g\" ${CMAKE_CURRENT_SOURCE_DIR}/test_configuration_template.json > ${CMAKE_CURRENT_BINARY_DIR}/test_configuration.json
)

set(TEST_CONFIGURATION_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

add_custom_target(test_configuration
                  DEPENDS test_configuration.json)