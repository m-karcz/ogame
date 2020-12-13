cmake_minimum_required(VERSION 3.16)

add_library(Logic Service.hpp Service.cpp handlers/BuildRequestHandler.cpp handlers/BuildRequestHandler.hpp handlers/StorageRequestHandler.cpp handlers/StorageRequestHandler.hpp handlers/BuildingsListRequestHandler.cpp handlers/BuildingsListRequestHandler.hpp handlers/BuildingQueueRequestHandler.hpp handlers/BuildingQueueRequestHandler.cpp)

add_library(Logic::Logic ALIAS Logic)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Logic PROPERTY CXX_STANDARD 17)
target_include_directories(Logic PUBLIC .)
target_link_libraries(Logic Data::Data Json::Json DatabaseInterface::DatabaseInterface)

