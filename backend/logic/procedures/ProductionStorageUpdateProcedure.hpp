#pragma once

#include "Timestamp.hpp"
#include "SinglePlanetContext.hpp"


void productionStorageUpdateProcedure(SinglePlanetContext& ctx, Timestamp until);

inline void productionStorageUpdateProcedure(SinglePlanetContext& ctx)
{
    productionStorageUpdateProcedure(ctx, ctx.timestamp);
}