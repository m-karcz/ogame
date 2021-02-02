#pragma once

#include "ISerializer.hpp"

struct JsonSerializer : ISerializer
{
    std::vector<uint8_t> serialize(const SerializableResponse&) const override;
    SerializableRequest deserialize(const std::vector<uint8_t>&) const override;
};