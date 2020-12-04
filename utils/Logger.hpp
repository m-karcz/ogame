#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/file_sinks.h>

struct Logger
{
    Logger()
    {
        loggerPtr->set_level(spdlog::level::debug);
        loggerPtr->flush_on(spdlog::level::debug);
    }
    std::shared_ptr<spdlog::logger> loggerPtr = spdlog::basic_logger_st("basic","eloelo.txt");
};

inline Logger loggerInstance;
inline spdlog::logger& logger = *loggerInstance.loggerPtr;