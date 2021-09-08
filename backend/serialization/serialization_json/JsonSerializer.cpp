#include "JsonSerializer.hpp"
#include "toJson.hpp"
#include "Logger.hpp"
#include "WorkSharedPlanetRequest.hpp"
#include "AuthenticatedOnPlanetRequest.hpp"
#include "InternalGeneralRequest.hpp"
#include "InternalGeneralResponse.hpp"
#include "LockRequestNew.hpp"
#include "LockResponseNew.hpp"
#include <iostream>

template<typename T>
Payload JsonTypedSerializer<T>::serialize(const T& value) const
{
    Json j = serializeFrom(value);
    auto dumped = j.dump();
    return {dumped.begin(), dumped.end()};
}

template<typename T>
T JsonTypedDeserializer<T>::deserialize(const Payload& payload) const
{
    Json j = Json::parse(payload.begin(), payload.end());
    return deserializeTo<T>(j);
}

template struct JsonTypedDeserializer<std::string>;
template struct JsonTypedSerializer<std::string>;
template struct JsonTypedDeserializer<OnPlanetResponseNew>;
template struct JsonTypedSerializer<OnPlanetResponseNew>;
template struct JsonTypedDeserializer<AuthenticatedOnPlanetRequest>;
template struct JsonTypedSerializer<AuthenticatedOnPlanetRequest>;
template struct JsonTypedDeserializer<WorkSharedPlanetRequest>;
template struct JsonTypedSerializer<WorkSharedPlanetRequest>;
template struct JsonTypedSerializer<InternalGeneralResponse>;
template struct JsonTypedDeserializer<InternalGeneralRequest>;
template struct JsonTypedSerializer<LockRequestNew>;
template struct JsonTypedSerializer<LockResponseNew>;
template struct JsonTypedDeserializer<LockRequestNew>;
template struct JsonTypedDeserializer<LockResponseNew>;

template struct JsonTypedDeserializer<InternalGeneralResponse>;
template struct JsonTypedSerializer<InternalGeneralRequest>;