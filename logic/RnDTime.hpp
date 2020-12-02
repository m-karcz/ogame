#pragma once

#include "ITime.hpp"
#include "TimeForwardRequest.hpp"

using namespace std::chrono_literals;

struct RnDTime : ITime
{
    void setTimeTo(const Timepoint& timepoint) override
    {
        actualTime = timepoint;
    }
    Timepoint getTimestamp() override
    {
        return actualTime;
    }
    void requestShifting(const TimeForwardRequest& req)
    {
        actualTime += req.duration;
    }
    Timepoint actualTime = Timepoint{Duration{0}};
};