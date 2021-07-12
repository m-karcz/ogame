#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"
#include "ProductionRecalculationProcedure.hpp"
#include "ProductionStorageUpdateProcedure.hpp"
#include "Constants.hpp"
#include "BuildingQueue.hpp"
#include "KnowledgeData.hpp"
#include "SinglePlanetContext.hpp"
#include "ProductionPercentages.hpp"
#include "CachedProduction.hpp"

struct EvaluateTimelineProcedure : SinglePlanetContext
{
    EvaluateTimelineProcedure(const SinglePlanetContext& ctx) : SinglePlanetContext{ctx}
    {
        if(auto queue = getBuildingQueue())
        {
            finishBuilding(*queue);
        }
        productionStorageUpdateProcedure(*this);
    }
    std::optional<BuildingQueue> getBuildingQueue()
    {
        if(auto queue = planet.getBuildingQueue())
        {
            if(queue->finishAt < timestamp)
            {
                return queue;
            }
        }
        return std::nullopt;
    }
    void finishBuilding(const BuildingQueue& queue)
    {
        productionStorageUpdateProcedure(*this, queue.finishAt);
        planet.dequeueBuilding(queue);
        planet.incrementBuildingLevel(queue.building);
        productionRecalculationProcedure(*this);
    }
};

void evaluteTimelineProcedure(const SinglePlanetContext& ctx)
{
    EvaluateTimelineProcedure{ctx};
}
