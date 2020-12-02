#pragma once

#include "BigNum.hpp"
#include <optional>

struct Production
{
    BigNum rawProduction;
    BigNum energyCost;
    BigNum realProduction;
};

struct CalculatedProduction
{
    BigNum baseMetal;
    BigNum baseCrystal;
    std::optional<Production> metalMine;
    std::optional<Production> crystalMine;
    std::optional<Production> deuterExt;
    std::optional<Production> solarGen;
};