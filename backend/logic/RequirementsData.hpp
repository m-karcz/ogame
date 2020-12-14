#pragma once

#include "Requirement.hpp"

struct RequirementsData
{
    RequirementsData();
    std::unordered_map<Research, Requirement> researchs;
    std::unordered_map<Building, Requirement> buildings;
};