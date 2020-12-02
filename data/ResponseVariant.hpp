#pragma once

#include <variant>

struct RegisterResponse;
struct LoginResponse;
struct StorageResponse;
struct BuildResponse;
struct TimeForwardResponse;

using ResponseVariant = std::variant<TimeForwardResponse>;