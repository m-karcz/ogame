#pragma once

#include "ISerializer.hpp"

template<typename T>
struct JsonTypedSerializer : ITypedSerializer<T>
{
    Payload serialize(const T&) const override;
};

template<typename T>
struct JsonTypedDeserializer : ITypedDeserializer<T>
{
    T deserialize(const Payload&) const override;
};
