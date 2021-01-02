cmake_minimum_required(VERSION 3.16)

project(ogame)

set(RUN_ROUTER "${CMAKE_BINARY_DIR}/runRouter")

add_subdirectory(common)
add_subdirectory(backend)
add_subdirectory(robot_tests)
add_subdirectory(router)
add_subdirectory(frontend)