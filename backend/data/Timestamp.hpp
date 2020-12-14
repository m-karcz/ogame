#pragma once

#include <chrono>

/*struct Timestamp
{
    constexpr static bool genEqOp = true;
    int value;
};

BOOST_HANA_ADAPT_STRUCT(Timestamp, value);*/

using Duration = std::chrono::milliseconds;
using Timestamp = std::chrono::time_point<std::chrono::steady_clock, Duration>;

