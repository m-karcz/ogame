#pragma once
#include <memory>
#include "SerializableRequest.hpp"
#include "SerializableResponse.hpp"
#include "Payload.hpp"

struct ISerializer
{
    virtual ~ISerializer() = default;
    virtual Payload serialize(const SerializableResponse&) const = 0;
    virtual SerializableRequest deserialize(const Payload&) const = 0;
};