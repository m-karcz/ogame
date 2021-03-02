
add_subdirectory(test_configuration)

find_package (Python3 COMPONENTS Interpreter Development)
find_program(VIRTUALENV virtualenv)
if(NOT VIRTUALENV)
    message(FATAL_ERROR "Could not find `virtualenv` in PATH")
endif()

add_custom_command(
    OUTPUT venv
    COMMAND ${VIRTUALENV} venv
)

add_custom_command(
    OUTPUT venv.stamp
    DEPENDS venv requirements.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt requirements.txt
    COMMAND ./venv/bin/pip install -r requirements.txt --upgrade
    COMMAND touch venv.stamp
)

add_custom_target(robot_tests
        COMMAND
            ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python -m robot --variable runRouter:${RUN_ROUTER} --variable testConfiguration:${TEST_CONFIGURATION_PATH}/test_configuration.json basic.robot
        DEPENDS
            venv.stamp
            prepare_router
            test_configuration
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
