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
    std::cout << __PRETTY_FUNCTION__ << ": trial to deserialize: " << j.dump() << std::endl;
    return deserializeTo<T>(j);
}

/*template<>
Payload JsonTypedSerializer<std::string>::serialize(const std::string&) const;

template<>
std::string JsonTypedDeserializer<std::string>::deserialize(const Payload&) const;

template<>
Payload JsonTypedSerializer<OnPlanetResponseNew>::serialize(const OnPlanetResponseNew&) const;

template<>
OnPlanetResponseNew JsonTypedDeserializer<OnPlanetResponseNew>::deserialize(const Payload&) const;

template<>
Payload JsonTypedSerializer<AuthenticatedOnPlanetRequest>::serialize(const AuthenticatedOnPlanetRequest&) const;

template<>
AuthenticatedOnPlanetRequest JsonTypedDeserializer<AuthenticatedOnPlanetRequest>::deserialize(const Payload&) const;

template<>
Payload JsonTypedSerializer<WorkSharedPlanetRequest>::serialize(const WorkSharedPlanetRequest&) const;

template<>
WorkSharedPlanetRequest JsonTypedDeserializer<WorkSharedPlanetRequest>::deserialize(const Payload&) const;*/

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