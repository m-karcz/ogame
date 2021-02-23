#include "ProductionStorageUpdateProcedure.hpp"
#include "KnowledgeData.hpp"
#include "CachedProduction.hpp"
#include "IPlanetHandle.hpp"
#include "Storage.hpp"
#include "Logger.hpp"

void productionStorageUpdateProcedure(SinglePlanetContext& ctx, Timestamp until)
{
    CachedProduction prod = ctx.planet.getCachedProduction();
    Storage storage = ctx.planet.getStorage();
    BigNum hourlyMetal = knowledgeData.productions.baseMetal + prod.metalMineGeneration;
    BigNum hourlyCrystal = knowledgeData.productions.baseCrystal + prod.crystalMineGeneration;
    BigNum hourlyDeuter = prod.deuterExtractorGeneration - prod.fusionGeneratorDeuterUsage;
    logger.debug("production: metal {} crystal {} deuter {}", hourlyMetal.toString(), hourlyCrystal.toString(), hourlyDeuter.toString());
    if(hourlyDeuter < 0)
    {
        hourlyDeuter = 0;
    }
    using HourInFloat = std::chrono::duration<double, std::ratio<3600>>;

    HourInFloat duration = until - storage.lastUpdatedAt;
    logger.debug("production over {} h", duration.count());
    storage.metal = storage.metal + duration.count() * hourlyMetal;
    storage.crystal = storage.crystal + duration.count() * hourlyCrystal;
    storage.deuter = storage.deuter + duration.count() * hourlyDeuter;
    ctx.planet.setNewStorage(storage);
}