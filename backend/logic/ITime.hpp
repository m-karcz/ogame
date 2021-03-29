#pragma once

#include <chrono>
#include "Timestamp.hpp"
#include "Duration.hpp"


struct ITime
{
    virtual void setTimeTo(const Timestamp&) = 0;
    virtual void shiftTimeBy(const Duration&) = 0;
    virtual Timestamp getTimestamp() = 0;
    virtual ~ITime() = default;
};

