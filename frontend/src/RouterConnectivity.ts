import {UserCredentials} from "./UserCredentials"
import {LoginResponse} from "./LoginResponse"
import {RegisterResponse} from "./RegisterResponse"
import IRouterConnectivity from "./IRouterConnectivity"
import { PlanetLocation } from "./PlanetLocation"
import { OverviewViewResponse } from "./OverviewViewResponse"
import { OverviewViewRequest } from "./OverviewViewRequest"
import { BuildingsViewRequest} from "./BuildingsViewRequest"
import { BuildingsViewResponse} from "./BuildingsViewResponse"
import { StartBuildingActionRequest } from "./StartBuildingActionRequest"
import { StartBuildingActionResponse } from "./StartBuildingActionResponse"
import { Building } from "./Building"

function withPayload(payload: any)
{
    console.log(payload)
    return {
        method: "POST",
        body: JSON.stringify(payload),
        credentials: "include" as RequestCredentials,
        headers: {
            'Content-Type': 'application/json'
        }
    }
}

async function fetchAs<T>(dest: string, payload: any) : Promise<T>
{
    const resp = await fetch(dest, withPayload(payload));
    return await resp.json() as T;
}

export default class RouterConnectivity implements IRouterConnectivity
{
    tryLogin(credentials: UserCredentials) : Promise<LoginResponse>
    {
        return fetchAs<LoginResponse>("/login", credentials);
    }
    tryRegister(credentials: UserCredentials) : Promise<RegisterResponse>
    {
        return fetchAs<RegisterResponse>("/register", credentials);
    }
    loadOverviewPage(planet: PlanetLocation) : Promise<OverviewViewResponse>
    {
        return fetchAs<OverviewViewResponse>("/game/overview", {planet: planet} as OverviewViewRequest);
    }
    loadBuildingsPage(planet: PlanetLocation) : Promise<BuildingsViewResponse>
    {
        return fetchAs<BuildingsViewResponse>("/game/buildings", {planet: planet} as BuildingsViewRequest);
    }
    beginBuilding(planet: PlanetLocation, building: Building) : Promise<StartBuildingActionResponse>
    {
        return fetchAs<StartBuildingActionResponse>("/game/startBuilding", {planet: planet, building: building} as StartBuildingActionRequest);
    }
}