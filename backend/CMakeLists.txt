
if(EMSCRIPTEN)
    add_compile_options("-sDISABLE_EXCEPTION_CATCHING=0")
    add_link_options("-sDISABLE_EXCEPTION_CATCHING=0")
    add_compile_options("-sALLOW_MEMORY_GROWTH=1")
    add_link_options("-sALLOW_MEMORY_GROWTH=1")
endif()
include(CMakeLists.txt.externals)
find_package(Boost 1.65 COMPONENTS program_options OPTIONAL)

add_subdirectory(utils)
add_subdirectory(data)
add_subdirectory(configuration)
add_subdirectory(knowledge)
add_subdirectory(serialization)
add_subdirectory(tests)
add_subdirectory(logic)
add_subdirectory(database)
add_subdirectory(rnd)
add_subdirectory(deployments)





