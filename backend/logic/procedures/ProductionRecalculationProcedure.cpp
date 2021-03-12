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
    PartialProduction deuteriumSynthesizer;
    PartialProduction solarPlant;
    PartialProduction solarSatelite;
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
    PartialProduction theoreticalDeuteriumSynthesizer()
    {
        auto averageTemp = 0;
        auto level = planet.getBuildingLevel(cnst::deuteriumSynthesizer);
        auto data = knowledgeData.productions.deuteriumSynthesizer;
        return {
            .producing = data.production * level * std::pow(1.1, level) * (0.68 - 0.002 * averageTemp),
            .cost = data.cost * level * std::pow(1.1, level)
        };
    }
    PartialProduction theoreticalSolarPlant()
    {
        auto level = planet.getBuildingLevel(cnst::solarPlant);
        auto data = knowledgeData.productions.solarPlant;
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
            .deuteriumSynthesizer = theoreticalDeuteriumSynthesizer(),
            .solarPlant = theoreticalSolarPlant(),
            .solarSatelite = {0.0, 0.0},
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
            .deuteriumSynthesizer = applyPercentage(th.deuteriumSynthesizer, percentage.deuteriumSynthesizer),
            .solarPlant = applyPercentage(th.solarPlant, percentage.solarPlant),
            .solarSatelite = applyPercentage(th.solarSatelite, percentage.solarSatelite),
            .fusionReactor = applyPercentage(th.fusionReactor, percentage.fusionReactor)
        };
    }

    Efficiency evaluateEfficiency(const TheoreticalProduction& prod)
    {
        const auto safeDiv = [](double x, double y)->double{
            if(x < 0.01 and x > -0.01)
            {
                return 0.0;
            }
            if(y < 0.01 and y > -0.01)
            {
                return 1.0;
            }
            return x / y;
        };
        Efficiency eff{0.5, 0.5};
        auto deuProd = prod.deuteriumSynthesizer.producing;
        auto fusionDeuCost = prod.fusionReactor.cost;
        auto fusionEnProd = prod.fusionReactor.producing;
        auto mineEnCost = prod.metalMine.cost + prod.crystalMine.cost + prod.deuteriumSynthesizer.cost;
        if(mineEnCost < 0.1)
        {
            return {1.0, 1.0};
        }
        auto solarEnProd = prod.solarPlant.producing + prod.solarSatelite.producing;

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
            .metalMineIdealGeneration = toInt(prod.metalMine.producing),
            .metalMineEnergyUsage = toInt(prod.metalMine.cost),
            .crystalMineGeneration = toInt(prod.crystalMine.producing * eff.energy),
            .crystalMineIdealGeneration = toInt(prod.crystalMine.producing),
            .crystalMineEnergyUsage = toInt(prod.crystalMine.cost),
            .deuteriumSynthesizerGeneration = toInt(prod.deuteriumSynthesizer.producing * eff.energy),
            .deuteriumSynthesizerIdealGeneration = toInt(prod.deuteriumSynthesizer.producing),
            .deuteriumSynthesizerEnergyUsage = toInt(prod.deuteriumSynthesizer.cost),
            .solarPlantGeneration = toInt(prod.solarPlant.producing),
            .solarSateliteGeneration = toInt(prod.solarSatelite.producing),
            .fusionReactorGeneration = toInt(prod.fusionReactor.producing * eff.deuter),
            .fusionReactorIdealGeneration = toInt(prod.fusionReactor.producing),
            .fusionReactorDeuterUsage = toInt(prod.fusionReactor.cost)
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