cmake_minimum_required(VERSION 3.16)

enable_testing()

set(CMAKE_CXX_STANDARD 17)




add_executable(ogame_tests BigNumTest.cpp)
target_link_libraries(ogame_tests gtest gtest_main Utils::Utils)
add_test(NAME ogame_tests COMMAND ogame_tests)