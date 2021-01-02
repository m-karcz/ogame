#include "StorageRequestHandler.hpp"
#include "StorageRequest.hpp"
#include "StorageResponse.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"

StorageResponse StorageRequestHandler::handleQuery(const StorageRequest& req)
{
    logger.debug("Querying storage");
    return {planet.getStorage()};
}
