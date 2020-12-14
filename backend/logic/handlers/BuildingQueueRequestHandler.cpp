#include "BuildingQueueRequestHandler.hpp"
#include "api/BuildingQueueRequest.hpp"
#include "BuildingQueue.hpp"
#include "IPlanetHandle.hpp"

BuildingQueueResponse BuildingQueueRequestHandler::handleQuery(const BuildingQueueRequest& req)
{
    BuildingQueueResponse resp{};
    if(auto queue = planet.getBuildingQueue())
    {
        resp.queue = BuildingQueueEntry{
                .building = queue->building,
                .timeToFinish = Duration{queue->finishAt - timestamp}
        };
    }
    return resp;
}

