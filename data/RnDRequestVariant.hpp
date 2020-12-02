#pragma once

#include <variant>

struct TimeForwardRequest;

using RnDRequestVariant = std::variant<TimeForwardRequest>;