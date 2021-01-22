#pragma once

#include "Storage.hpp"
#include "CalculatedProduction.hpp"
#include "Buildings.hpp"
#include "Logger.hpp"

struct ProductionCalculator
{
    struct MineData
    {
        BigNum baseProduction;
        BigNum productionMultiplier;
        BigNum energyTaken;
        BigNum energyMultiplier;
    };

    BigNum rawProduction(int level, const MineData& data)
    {
        return level > 0 ? data.baseProduction * pow(data.productionMultiplier, level - 1) : 0;
    }

    BigNum energyUsage(int level, const MineData& data)
    {
        logger.debug(__PRETTY_FUNCTION__);
        logger.debug(data.energyTaken.toString());
        logger.debug(level);
        logger.debug(data.energyMultiplier.toString());
        logger.debug(pow(data.energyMultiplier,level).toString());
        return data.energyTaken * pow(data.energyMultiplier, level);
    }

    CalculatedProduction calc(const Buildings& buildings)
    {
        logger.debug("elo1");
        MineData metalData   = {.baseProduction = 30, .productionMultiplier = 1.1, .energyTaken = 10, .energyMultiplier = 1.1};
        MineData crystalData = {.baseProduction = 20, .productionMultiplier = 1.1, .energyTaken = 10, .energyMultiplier = 1.1};
        MineData deuterData  = {.baseProduction = 10, .productionMultiplier = 1.1, .energyTaken = 10, .energyMultiplier = 1.1};
        MineData solarData   = {.baseProduction = 20, .productionMultiplier = 1.1, .energyTaken = 0,  .energyMultiplier = 1};

        logger.debug("elo2");
        BigNum rawMetal = rawProduction(buildings.metalMine, metalData);
        BigNum rawCrystal = rawProduction(buildings.crystalMine, crystalData);
        BigNum rawDeuter = rawProduction(buildings.deuterExtractor, deuterData);
        BigNum rawSolar = rawProduction(buildings.solarGenerator, solarData);

        logger.debug("elo3");
        BigNum producedEnergy = rawSolar;
        logger.debug("elo31");
        BigNum metalEnergyUsage = energyUsage(buildings.metalMine, metalData);
        logger.debug("elo32");
        BigNum crystalEnergyUsage = energyUsage(buildings.crystalMine, crystalData);
        logger.debug("elo33");
        BigNum deuterEnergyUsage = energyUsage(buildings.deuterExtractor, deuterData);
        logger.debug("elo34");
        BigNum takenEnergy = metalEnergyUsage + crystalEnergyUsage + deuterEnergyUsage;
        logger.debug("elo4");

        BigNum energyRatio = producedEnergy / takenEnergy;

        if(1.0 < energyRatio)
        {
            energyRatio = 1.0;
        }
        logger.debug("elo5");

        return {
            .baseMetal = 20,
            .baseCrystal = 10,
            .metalMine = Production{
                .rawProduction = rawMetal,
                .energyCost = metalEnergyUsage,
                .realProduction = rawMetal * energyRatio
            },
            .crystalMine = Production{
                .rawProduction = rawCrystal,
                .energyCost = crystalEnergyUsage,
                .realProduction = rawCrystal * energyRatio
            },
            .deuterExt = Production{
                .rawProduction = rawDeuter,
                .energyCost = deuterEnergyUsage,
                .realProduction = rawDeuter * energyRatio
            },
            .solarGen = Production{
                .rawProduction = rawSolar,
                .energyCost = 0,
                .realProduction = rawSolar
            }
        };
    }

    void updateStorage(Storage& storage, Timestamp at, const Buildings& buildings)
    {
        logger.debug("elo11");

        const Production NOT_BUILD = {.rawProduction = 0, .energyCost = 0, .realProduction = 0};

        auto production = calc(buildings);

        logger.debug("elo12");

        auto duration = at - storage.lastUpdatedAt;


        using HourInFloat = std::chrono::duration<double, std::ratio<3600>>;
        logger.debug("elo13");

        HourInFloat partOfHour = duration;

        storage.metal = storage.metal + (production.baseMetal + production.metalMine.value_or(NOT_BUILD).realProduction) * partOfHour.count();
        storage.crystal = storage.crystal + (production.baseCrystal + production.crystalMine.value_or(NOT_BUILD).realProduction) * partOfHour.count();
        storage.deuter = storage.deuter + production.deuterExt.value_or(NOT_BUILD).realProduction * partOfHour.count();

        logger.debug("elo14");

        storage.lastUpdatedAt = at;
    }

};

