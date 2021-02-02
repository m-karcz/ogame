#pragma once
#include <memory>
#include "SerializableRequest.hpp"
#include "SerializableResponse.hpp"

struct ISerializer
{
    virtual ~ISerializer() = default;
    virtual std::vector<uint8_t> serialize(const SerializableResponse&) const = 0;
    virtual SerializableRequest deserialize(const std::vector<uint8_t>&) const = 0;
};