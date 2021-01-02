
set(OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

#add_custom_command(generate_common
#                   COMMAND mkdir -p ${OUTDIR}
#                   COMMAND python3 generator.py ${CMAKE_CURRENT_BINARY_DIR}/generated
#                   DEPENDS
#                        types.yaml
#                        generator.py)

execute_process(COMMAND mkdir -p \\"${OUTDIR}\\"
                COMMAND python3 generator.py ${OUTDIR}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${OUTDIR} ${OUTDIR})

