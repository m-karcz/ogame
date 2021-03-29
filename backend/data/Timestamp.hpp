#pragma once

#include <chrono>
#include "Duration.hpp"

using Timestamp = std::chrono::time_point<std::chrono::system_clock, Duration>;

