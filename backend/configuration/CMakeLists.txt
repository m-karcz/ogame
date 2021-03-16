add_library(Configuration LoadConfiguration.hpp LoadConfiguration.cpp)
add_library(Configuration::Configuration ALIAS Configuration)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Configuration PROPERTY CXX_STANDARD 17)
target_include_directories(Configuration PUBLIC .)
target_link_libraries(Configuration Data::Data SerializationJson::SerializationJson Utils::Utils)