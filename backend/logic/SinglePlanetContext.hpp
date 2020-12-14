#pragma once

struct IPlayerHandle;
struct IPlanetHandle;
#include "Timestamp.hpp"

struct SinglePlanetContext
{
    IPlayerHandle& player;
    IPlanetHandle& planet;
    Timestamp timestamp;
};

