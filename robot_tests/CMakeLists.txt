
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
    COMMAND ./venv/bin/pip --proxy http://defra1c-proxy.emea.nsn-net.net:8080 install -r requirements.txt --upgrade
)

add_custom_target(robot_tests
        COMMAND
            ${CMAKE_CURRENT_BINARY_DIR}/venv/bin/python -m robot --variable serverFile:$<TARGET_FILE:server> basic.robot
        DEPENDS
            venv.stamp
            server
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})