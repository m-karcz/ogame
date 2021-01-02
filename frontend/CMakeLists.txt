execute_process(COMMAND npm install
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(frontend
    DEPENDS
    COMMAND npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR})
