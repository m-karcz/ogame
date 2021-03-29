#pragma once

#include "ITime.hpp"
#include "TimeForwardRequest.hpp"
#include "Logger.hpp"

using namespace std::chrono_literals;

struct RnDTime : ITime
{
    void setTimeTo(const Timestamp& timepoint) override
    {
        actualTime = timepoint;
    }
    Timestamp getTimestamp() override
    {
        logger.debug("requested time value, now: {}", actualTime.time_since_epoch().count());
        return actualTime;
    }
    void shiftTimeBy(const Duration& duration) override
    {
        setTimeTo(getTimestamp() + duration);
    }
    void requestShifting(const TimeForwardRequest& req)
    {
        logger.debug("shifted time by {}", req.duration.count());
        actualTime += req.duration;
    }
    Timestamp actualTime = Timestamp{Duration{0}};
};