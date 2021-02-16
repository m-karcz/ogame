cmake_minimum_required(VERSION 3.16)

#taken from https://stackoverflow.com/questions/410980/include-a-text-file-in-a-c-program-as-a-char
function(make_includable input_file output_file)
    file(READ ${input_file} content)
    set(delim "for_c++_include")
    set(content "R\"${delim}(\n${content})${delim}\"")
    file(WRITE ${output_file} "${content}")
endfunction(make_includable)

set(INCLUDABLE_KNOWLEDGE_PATH ${CMAKE_CURRENT_BINARY_DIR}/Knowledge.json.wrapped)

#get_property(KNOWLEDGE_PATH_PROPERTY GLOBAL PROPERTY KNOWLEDGE_PATH)

message(STATUS "no elo")
message(STATUS ${KNOWLEDGE_PATH})
message(STATUS ${INCLUDABLE_KNOWLEDGE_PATH})

make_includable(${KNOWLEDGE_PATH} ${INCLUDABLE_KNOWLEDGE_PATH})

add_library(Knowledge KnowledgeData.hpp KnowledgeData.cpp)
add_library(Knowledge::Knowledge ALIAS Knowledge)



set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Knowledge PROPERTY CXX_STANDARD 17)
target_include_directories(Knowledge PUBLIC .)
target_link_libraries(Knowledge Data::Data SerializationJson::SerializationJson)
target_compile_definitions(Knowledge PRIVATE KNOWLEDGE_PATH="${INCLUDABLE_KNOWLEDGE_PATH}")

