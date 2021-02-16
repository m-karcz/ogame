#pragma once
#include "Timestamp.hpp"

struct IPlayerHandle;
struct IPlanetHandle;

struct SinglePlanetContext
{
    IPlayerHandle& player;
    IPlanetHandle& planet;
    Timestamp timestamp;
};