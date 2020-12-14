#pragma once

#include <variant>
#include "LoginRequest.hpp"
#include "RegisterRequest.hpp"

using GeneralRequest = std::variant<LoginRequest, RegisterRequest>;

using GeneralResponse = std::variant<LoginResponse, RegisterResponse>;