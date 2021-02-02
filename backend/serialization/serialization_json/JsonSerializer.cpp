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
    logger.debug("Making json");
    Json j = Json::parse(stream.begin(), stream.end());
    logger.debug("Made json");
    logger.debug(j.dump());
    return deserializeTo<SerializableRequest>(j);
}