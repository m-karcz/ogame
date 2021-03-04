import {createAction} from "@reduxjs/toolkit"

import { UserCredentials,
         Buildings,
         PlanetLocation,
         GeneralContext,
         Building,
         BuildingQueueResponse,
        ProductionInformation } from "./generated/AllGenerated"

export const LOGIN_SUBMIT = "LOGIN_SUBMIT"
export const LOGIN_SUCCESSFUL = "LOGIN_SUCCESSFUL"
export const REGISTER_SUBMIT = "REGISTER_SUBMIT"
export const REGISTER_SUCCESSFUL = "REGISTER_SUCCESSFUL"
export const BUILDINGS_CLICKED = "BUILDINGS_CLICKED"

export type PlanetPayload = 
{
    planet: PlanetLocation
}

export type BuildingPayload =
{
    building: Building
}

export type BuildingsLoadedPayload = 
{
    buildings: Buildings
    queue: BuildingQueueResponse["queue"]
}

export type ChosenPlanetPayload =
{
    chosenPlanet: PlanetLocation
}

export const registerRequest = createAction<UserCredentials>("login/registerRequest");
export const registerSuccessful = createAction("login/registerSuccessful");
export const loginRequest = createAction<UserCredentials>("login/loginRequest");
export const loginSucceeded = createAction<ChosenPlanetPayload>("login/succeeded");

export const loadOverviewPage = createAction<PlanetPayload>("ingame/overviewPage");
export const loadBuildingsPage = createAction<PlanetPayload>("ingame/buildingsPage");
export const loadResourcesPage = createAction<PlanetPayload>("ingame/resourcesPage");
export const startBuilding = createAction<BuildingPayload>("ingame/startBuilding");
export const contextUpdated = createAction<GeneralContext>("ingame/contextUpdated");
export const overviewLoaded = createAction("ingame/overviewLoaded");
export const buildingsLoaded = createAction<BuildingsLoadedPayload>("ingame/buildingsLoaded");
export const resourcesLoaded = createAction<ProductionInformation>("ingame/resourcesLoaded");

interface WithContext
{
    context: GeneralContext
}

export function getContextUpdated(msg: WithContext)
{
    return contextUpdated(msg.context);
}
//export const ingamePageUpdated = createAction
