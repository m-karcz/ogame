#pragma once

#include "ITime.hpp"
#include "api/TimeForwardRequest.hpp"
#include "Logger.hpp"

using namespace std::chrono_literals;

struct RnDTime : ITime
{
    void setTimeTo(const Timepoint& timepoint) override
    {
        actualTime = timepoint;
    }
    Timepoint getTimestamp() override
    {
        logger.debug("requested time value, now: {}", actualTime.time_since_epoch().count());
        return actualTime;
    }
    void requestShifting(const TimeForwardRequest& req)
    {
        logger.debug("shifted time by {}", req.duration.count());
        actualTime += req.duration;
    }
    Timepoint actualTime = Timepoint{Duration{0}};
};