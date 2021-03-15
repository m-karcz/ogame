#include "JsonSerializer.hpp"
#include "toJson.hpp"
#include "Logger.hpp"

std::vector<uint8_t> JsonSerializer::serialize(const SerializableResponse& resp) const
{
    Json j = serializeFrom(resp);
    auto dumped = j.dump();
    return {dumped.begin(), dumped.end()};
}
SerializableRequest JsonSerializer::deserialize(const std::vector<uint8_t>& stream) const
{
    Json j = Json::parse(stream.begin(), stream.end());
    logger.trace(j.dump());
    return deserializeTo<SerializableRequest>(j);
}