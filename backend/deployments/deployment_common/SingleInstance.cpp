#include "SingleInstance.hpp"
#include "Overloaded.hpp"
#include <variant>
#include "SerializableResponse.hpp"
#include "SerializableRequest.hpp"

SingleInstance::SingleInstance(const ISerializer& serializer, const Configuration& configuration, ITime& time, IStorageDbFactory& storageDbFactory) try
    : serializer{serializer}
    , configuration{configuration}
    , storageDb{storageDbFactory.create()}
    , time{time}
    , service{*storageDb,time}
    , rndService{time}
{

}
catch(std::exception& ex)
{

}
catch(...)
{

}

std::vector<uint8_t> SingleInstance::process(const std::vector<uint8_t>& rawData)
{
    SerializableRequest request = serializer.deserialize(rawData);

    logger.debug("Deserialized");

    auto resp = std::visit(overloaded{
        [&,this](const OnPlanetRequest& req){return SerializableResponse{this->service.handleSinglePlanetRequest(req), request.transactionId};},
        [&,this](const GeneralRequest& req) {return SerializableResponse{this->service.handleRequest(req), request.transactionId};},
        [&,this](const RndRequest& req)     {return SerializableResponse{this->rndService.handleRequest(req), request.transactionId};}
    }, request.request);

    logger.debug("Serializing");

    return serializer.serialize(resp);
}