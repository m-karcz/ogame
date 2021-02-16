cmake_minimum_required(VERSION 3.16)

add_library(Logic
            Service.hpp
            Service.cpp
            handlers/BuildRequestHandler.cpp
            handlers/BuildRequestHandler.hpp
            handlers/StorageRequestHandler.cpp
            handlers/StorageRequestHandler.hpp
            handlers/BuildingsListRequestHandler.cpp
            handlers/BuildingsListRequestHandler.hpp
            handlers/BuildingQueueRequestHandler.hpp
            handlers/BuildingQueueRequestHandler.cpp
            handlers/ProductionInformationRequestHandler.hpp
            handlers/ProductionInformationRequestHandler.cpp
            handlers/ProductionPercentagesRequestHandler.hpp
            handlers/ProductionPercentagesRequestHandler.cpp
            procedures/ProductionRecalculationProcedure.hpp
            procedures/ProductionRecalculationProcedure.cpp
            procedures/ProductionStorageUpdateProcedure.hpp
            procedures/ProductionStorageUpdateProcedure.cpp
            RequirementsData.hpp
            RequirementsData.cpp
            Constants.hpp
            Constants.cpp)

add_library(Logic::Logic ALIAS Logic)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Logic PROPERTY CXX_STANDARD 17)
target_include_directories(Logic PUBLIC .)
target_link_libraries(Logic Data::Data DatabaseInterface::DatabaseInterface Knowledge::Knowledge)

