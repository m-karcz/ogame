#pragma once

#include "ISerializer.hpp"

struct JsonSerializer : ISerializer
{
    Payload serialize(const SerializableResponse&) const override;
    SerializableRequest deserialize(const Payload&) const override;
};