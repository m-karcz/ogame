#pragma once
#include "Timestamp.hpp"

struct IPlayerHandle;
struct IPlanetHandle;
struct Configuration;

struct SinglePlanetContext
{
    IPlayerHandle& player;
    IPlanetHandle& planet;
    Timestamp timestamp;
    const Configuration& configuration;
};