#pragma once

#include <chrono>
#include "Timestamp.hpp"

using Timepoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;
using Duration = std::chrono::milliseconds;

struct ITime
{
    virtual void setTimeTo(const Timestamp&) = 0;
    virtual Timestamp getTimestamp() = 0;
    virtual ~ITime() = default;
};

