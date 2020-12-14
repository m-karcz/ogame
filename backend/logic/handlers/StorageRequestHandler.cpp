#include "StorageRequestHandler.hpp"
#include "api/StorageRequest.hpp"
#include "IPlanetHandle.hpp"

StorageResponse StorageRequestHandler::handleQuery(const StorageRequest& req)
{
    return {planet.getStorage()};
}
