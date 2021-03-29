add_custom_command(
    OUTPUT test_configuration.json
    DEPENDS
        test_configuration_template.json
        rnd_caller 
    COMMAND
        sed \"s@RND_CALLER_PATH@${RND_CALLER_PATH}@g\" ${CMAKE_CURRENT_SOURCE_DIR}/test_configuration_template.json > ${CMAKE_CURRENT_BINARY_DIR}/test_configuration.json
)

set(TEST_CONFIGURATION_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")

add_custom_target(test_configuration
                  DEPENDS test_configuration.json)