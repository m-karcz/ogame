#pragma once

#include "Requirement.hpp"

struct RequirementsData
{
    std::unordered_map<Building, Requirement> buildings;
};