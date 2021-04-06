#pragma once

#include <stdexcept>

struct MiscError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

[[noreturn]] void throwMiscError(const std::string& msg)
{
    throw MiscError{msg};
}

[[noreturn]] void throwShouldNeverHappen(const std::string& msg)
{
    throwMiscError(msg);
}