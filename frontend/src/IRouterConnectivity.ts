import {UserCredentials} from "./UserCredentials"
import {LoginResponse} from "./LoginResponse"
import {RegisterResponse} from "./RegisterResponse"
import { PlanetLocation } from "./PlanetLocation"
import { OverviewViewResponse } from "./OverviewViewResponse"
import { BuildingsViewResponse} from "./BuildingsViewResponse"
import { Building } from "./Building"
import { StartBuildingActionResponse} from "./StartBuildingActionResponse"
import { TotalProductionViewResponse } from "./TotalProductionViewResponse"

export default interface IRouterConnectivity
{
    tryLogin(credentials: UserCredentials) : Promise<LoginResponse>
    tryRegister(credentials: UserCredentials) : Promise<RegisterResponse>
    loadOverviewPage(planet: PlanetLocation) : Promise<OverviewViewResponse>
    loadBuildingsPage(planet: PlanetLocation) : Promise<BuildingsViewResponse>
    loadResourcesPage(planet: PlanetLocation) : Promise<TotalProductionViewResponse>
    beginBuilding(planet: PlanetLocation, building: Building) : Promise<StartBuildingActionResponse>
}