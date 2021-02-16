#include "ProductionInformationRequestHandler.hpp"
#include "ProductionInformationRequest.hpp"
#include "ProductionInformationResponse.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"

ProductionInformationResponse ProductionInformationRequestHandler::handleQuery(const ProductionInformationRequest& req)
{
    logger.debug("Querying production");
    return {planet.getCachedProduction()};
}
