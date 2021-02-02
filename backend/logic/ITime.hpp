#pragma once

#include <chrono>
#include "Timestamp.hpp"

using Timepoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::seconds>;
using Duration = std::chrono::seconds;

struct ITime
{
    virtual void setTimeTo(const Timestamp&) = 0;
    virtual void shiftTimeBy(const Duration&) = 0;
    virtual Timestamp getTimestamp() = 0;
    virtual ~ITime() = default;
};

