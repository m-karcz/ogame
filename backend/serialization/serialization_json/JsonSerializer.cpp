#include "JsonSerializer.hpp"
#include "toJson.hpp"
#include "Logger.hpp"

Payload JsonSerializer::serialize(const SerializableResponse& resp) const
{
    Json j = serializeFrom(resp);
    auto dumped = j.dump();
    return {dumped.begin(), dumped.end()};
}
SerializableRequest JsonSerializer::deserialize(const Payload& stream) const
{
    Json j = Json::parse(stream.begin(), stream.end());
    logger.trace(j.dump());
    return deserializeTo<SerializableRequest>(j);
}