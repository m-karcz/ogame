#include "BuildingsListRequestHandler.hpp"
#include "api/BuildingsListRequest.hpp"
#include "IPlanetHandle.hpp"

BuildingsListResponse BuildingsListRequestHandler::handleQuery(const BuildingsListRequest& req)
{
    return {planet.getBuildings()};
}
