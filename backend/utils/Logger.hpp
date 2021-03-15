#pragma once

#include <spdlog/spdlog.h>

struct Logger
{
    Logger()
    {
        loggerPtr->set_level(spdlog::level::debug);
        loggerPtr->flush_on(spdlog::level::debug);
    }
    std::shared_ptr<spdlog::logger> loggerPtr = chooseLogger();
    static std::shared_ptr<spdlog::logger> chooseLogger();
};

inline Logger loggerInstance;
inline spdlog::logger& logger = *loggerInstance.loggerPtr;