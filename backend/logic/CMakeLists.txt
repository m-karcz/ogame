cmake_minimum_required(VERSION 3.16)

add_library(Logic
            AccountService.cpp
            AccountServiceApp.cpp
            LogicFsm.hpp
            LogicFsm.cpp
            LogicApp.hpp
            LogicApp.cpp
            
            handlers/BuildRequestHandler.cpp
            handlers/BuildRequestHandler.hpp
            procedures/ProductionRecalculationProcedure.hpp
            procedures/ProductionRecalculationProcedure.cpp
            procedures/ProductionStorageUpdateProcedure.hpp
            procedures/ProductionStorageUpdateProcedure.cpp
            procedures/EvaluateTimelineProcedure.hpp
            procedures/EvaluateTimelineProcedure.cpp
            Constants.hpp
            Constants.cpp)

add_library(Logic::Logic ALIAS Logic)

set (CMAKE_CXX_STANDARD 17)
set_property(TARGET Logic PROPERTY CXX_STANDARD 17)
target_include_directories(Logic PUBLIC .)
target_link_libraries(Logic
                      Data::Data
                      DatabaseInterface::DatabaseInterface
                      Knowledge::Knowledge
                      range-v3::range-v3
                      Messaging::Messaging)

