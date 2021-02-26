import {UserCredentials,
        LoginResponse,
        RegisterResponse,
         PlanetLocation,
         OverviewViewResponse,
         BuildingsViewResponse,
         StartBuildingActionResponse } from "./generated/AllGenerated"
import {Building} from "./Building"

type TotalProductionViewResponse = {}

export default interface IRouterConnectivity
{
    tryLogin(credentials: UserCredentials) : Promise<LoginResponse>
    tryRegister(credentials: UserCredentials) : Promise<RegisterResponse>
    loadOverviewPage(planet: PlanetLocation) : Promise<OverviewViewResponse>
    loadBuildingsPage(planet: PlanetLocation) : Promise<BuildingsViewResponse>
    loadResourcesPage(planet: PlanetLocation) : Promise<TotalProductionViewResponse>
    beginBuilding(planet: PlanetLocation, building: Building) : Promise<StartBuildingActionResponse>
}