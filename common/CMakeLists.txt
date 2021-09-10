
set(OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

#add_custom_command(generate_common
#                   COMMAND mkdir -p ${OUTDIR}
#                   COMMAND python3 generator.py ${CMAKE_CURRENT_BINARY_DIR}/generated
#                   DEPENDS
#                        types.yaml
#                        generator.py)
execute_process(COMMAND mkdir -p ${OUTDIR})

set(GENERATED_PATH ${CMAKE_CURRENT_BINARY_DIR}/generated CACHE INTERNAL "")
execute_process(COMMAND mkdir -p ${GENERATED_PATH}/cpp
                COMMAND mkdir -p ${GENERATED_PATH}/ts
                COMMAND python3 generator.py ${GENERATED_PATH}
                COMMAND cp tsconfig.json ${GENERATED_PATH}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


add_subdirectory(knowledge)
add_subdirectory(configuration)
add_subdirectory(${OUTDIR}/cpp ${OUTDIR}/cpp)

