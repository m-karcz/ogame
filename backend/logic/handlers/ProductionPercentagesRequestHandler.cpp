#include "ProductionPercentagesRequestHandler.hpp"
#include "ProductionPercentagesRequest.hpp"
#include "ProductionPercentagesResponse.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"

ProductionPercentagesResponse ProductionPercentagesRequestHandler::handleQuery(const ProductionPercentagesRequest&)
{
    logger.debug("Querying production percentages");
    return {planet.getProductionPercentages()};
}
