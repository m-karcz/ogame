#include "BuildingsListRequestHandler.hpp"
#include "BuildingsListRequest.hpp"
#include "BuildingsListResponse.hpp"
#include "IPlanetHandle.hpp"

BuildingsListResponse BuildingsListRequestHandler::handleQuery(const BuildingsListRequest& req)
{
    return {planet.getBuildings()};
}
