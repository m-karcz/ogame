#include "IPlayerHandle.hpp"
#include "IPlanetHandle.hpp"
#include "Logger.hpp"
#include "ProductionRecalculationProcedure.hpp"
#include "Constants.hpp"
#include "KnowledgeData.hpp"
#include "SinglePlanetContext.hpp"
#include "ProductionPercentages.hpp"
#include "CachedProduction.hpp"


struct PartialProduction
{
    double producing;
    double cost;
};

struct TheoreticalProduction
{
    PartialProduction metalMine;
    PartialProduction crystalMine;
    PartialProduction deuterExtractor;
    PartialProduction solarGenerator;
    PartialProduction solarSatelites;
    PartialProduction fusionReactor;
};

struct Efficiency
{
    double energy;
    double deuter;
};

PartialProduction applyPercentage(const PartialProduction& before, int percentage)
{
    return {
        .producing = before.producing * percentage / 100.0,
        .cost      = before.cost      * percentage / 100.0
    };
}


struct ProductionRecalculationProcedure : SinglePlanetContext
{
    PartialProduction theoreticalMetalMine()
    {
        auto level = planet.getBuildingLevel(cnst::metalMine);
        auto data = knowledgeData.productions.metalMine;
        return {
            .producing = data.production * level * std::pow(1.1, level),
            .cost = data.cost * level * std::pow(1.1, level)
        };
    }
    PartialProduction theoreticalCrystalMine()
    {
        auto level = planet.getBuildingLevel(cnst::crystalMine);
        auto data = knowledgeData.productions.crystalMine;
        return {
            .producing = data.production * level * std::pow(1.1, level),
            .cost = data.cost * level * std::pow(1.1, level)
        };
    }
    PartialProduction theoreticalDeuterExtractor()
    {
        auto averageTemp = 0;
        auto level = planet.getBuildingLevel(cnst::deuterExtractor);
        auto data = knowledgeData.productions.crystalMine;
        return {
            .producing = data.production * level * std::pow(1.1, level) * (0.68 - 0.002 * averageTemp),
            .cost = data.cost * level * std::pow(1.1, level)
        };
    }
    PartialProduction theoreticalSolarGenerator()
    {
        auto level = planet.getBuildingLevel(cnst::solarGenerator);
        auto data = knowledgeData.productions.solarGenerator;
        return {
            .producing = data.production * level * std::pow(1.1, level),
            .cost = 0.0
        };
    }
    TheoreticalProduction getTheoretical()
    {
        return {
            .metalMine = theoreticalMetalMine(),
            .crystalMine = theoreticalCrystalMine(),
            .deuterExtractor = theoreticalDeuterExtractor(),
            .solarGenerator = theoreticalSolarGenerator(),
            .solarSatelites = {0.0, 0.0},
            .fusionReactor = {0.0, 0.0}
        };
    }
    TheoreticalProduction getWithPercentages()
    {
        auto th = getTheoretical();
        auto percentage = planet.getProductionPercentages();
        return {
            .metalMine = applyPercentage(th.metalMine, percentage.metalMine),
            .crystalMine = applyPercentage(th.crystalMine, percentage.crystalMine),
            .deuterExtractor = applyPercentage(th.deuterExtractor, percentage.deuterExtractor),
            .solarGenerator = applyPercentage(th.solarGenerator, percentage.solarGenerator),
            .solarSatelites = applyPercentage(th.solarSatelites, percentage.solarSatelites),
            .fusionReactor = applyPercentage(th.fusionReactor, percentage.fusionReactor)
        };
    }

    Efficiency evaluateEfficiency(const TheoreticalProduction& prod)
    {
        const auto safeDiv = [](double x, double y)->double{
            if(y < 0.01 and y > -0.01)
            {
                return 1.0;
            }
            return x / y;
        };
        Efficiency eff{0.5, 0.5};
        auto deuProd = prod.deuterExtractor.producing;
        auto fusionDeuCost = prod.fusionReactor.cost;
        auto fusionEnProd = prod.fusionReactor.producing;
        auto mineEnCost = prod.metalMine.cost + prod.crystalMine.cost + prod.deuterExtractor.cost;
        if(mineEnCost)
        {
            return {1.0, 1.0};
        }
        auto solarEnProd = prod.solarGenerator.producing + prod.solarSatelites.producing;

        for(size_t i = 0; i < 5; i++)
        {
            Efficiency newEff{
                .energy = std::min(1.0, safeDiv(solarEnProd + eff.deuter * fusionEnProd, mineEnCost)),
                .deuter = std::min(1.0, safeDiv(deuProd * eff.energy, fusionDeuCost))
            };
            eff = newEff;
        }
        return eff;
    }

    CachedProduction calculateProduction()
    {
        auto prod = getWithPercentages();
        auto eff = evaluateEfficiency(prod);

        const auto toInt = [](double x){return static_cast<int>(x);};

        return {
            .metalMineGeneration = toInt(prod.metalMine.producing * eff.energy),
            .metalMineEnergyUsage = toInt(prod.metalMine.cost),
            .crystalMineGeneration = toInt(prod.crystalMine.producing * eff.energy),
            .crystalMineEnergyUsage = toInt(prod.crystalMine.cost),
            .deuterExtractorGeneration = toInt(prod.deuterExtractor.producing * eff.energy),
            .deuterExtractorEnergyUsage = toInt(prod.deuterExtractor.cost),
            .solarPlantGeneration = toInt(prod.solarGenerator.producing),
            .solarSateliteGeneration = toInt(prod.solarSatelites.producing),
            .fusionGeneratorGeneration = toInt(prod.fusionReactor.producing * eff.deuter),
            .fusionGeneratorDeuterUsage = toInt(prod.fusionReactor.cost)
        };
    }
    ProductionRecalculationProcedure(const SinglePlanetContext& ctx) : SinglePlanetContext{ctx}
    {
        planet.setNewCachedProduction(calculateProduction());
    }
};

void productionRecalculationProcedure(const SinglePlanetContext& ctx)
{
    ProductionRecalculationProcedure{ctx};
}