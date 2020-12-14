#pragma once

#include <variant>

struct RegisterRequest;
struct LoginRequest;
struct StorageRequest;
struct BuildRequest;
struct TimeForwardRequest;

//using RequestVariant = std::variant<std::monostate>;

struct RequestVariant
{};

