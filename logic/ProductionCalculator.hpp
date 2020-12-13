#pragma once

#include "Storage.hpp"
#include "CalculatedProduction.hpp"
#include "Buildings.hpp"

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
        return data.energyTaken * pow(data.energyMultiplier, level);
    }

    CalculatedProduction calc(const Buildings& buildings)
    {
        MineData metalData   = {.baseProduction = 30, .productionMultiplier = 1.1, .energyTaken = 10, .energyMultiplier = 1.1};
        MineData crystalData = {.baseProduction = 20, .productionMultiplier = 1.1, .energyTaken = 10, .energyMultiplier = 1.1};
        MineData deuterData  = {.baseProduction = 10, .productionMultiplier = 1.1, .energyTaken = 10, .energyMultiplier = 1.1};
        MineData solarData   = {.baseProduction = 20, .productionMultiplier = 1.1, .energyTaken = 0,  .energyMultiplier = 1};

        BigNum rawMetal = rawProduction(buildings.metalMine, metalData);
        BigNum rawCrystal = rawProduction(buildings.crystalMine, crystalData);
        BigNum rawDeuter = rawProduction(buildings.deuterExtractor, deuterData);
        BigNum rawSolar = rawProduction(buildings.solarGenerator, solarData);

        BigNum producedEnergy = rawSolar;
        BigNum metalEnergyUsage = energyUsage(buildings.metalMine, metalData);
        BigNum crystalEnergyUsage = energyUsage(buildings.crystalMine, crystalData);
        BigNum deuterEnergyUsage = energyUsage(buildings.deuterExtractor, deuterData);
        BigNum takenEnergy = metalEnergyUsage + crystalEnergyUsage + deuterEnergyUsage;

        BigNum energyRatio = producedEnergy / takenEnergy;

        if(1.0 < energyRatio)
        {
            energyRatio = 1.0;
        }

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

        const Production NOT_BUILD = {.rawProduction = 0, .energyCost = 0, .realProduction = 0};

        auto production = calc(buildings);

        auto duration = at - storage.lastUpdatedAt;


        using HourInFloat = std::chrono::duration<double, std::ratio<3600>>;

        HourInFloat partOfHour = duration;

        storage.metal = storage.metal + (production.baseMetal + production.metalMine.value_or(NOT_BUILD).realProduction) * partOfHour.count();
        storage.crystal = storage.crystal + (production.baseCrystal + production.crystalMine.value_or(NOT_BUILD).realProduction) * partOfHour.count();
        storage.deuter = storage.deuter + production.deuterExt.value_or(NOT_BUILD).realProduction * partOfHour.count();

        storage.lastUpdatedAt = at;
    }

};

