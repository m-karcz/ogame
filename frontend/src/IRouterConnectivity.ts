import {UserCredentials,
        LoginResponse,
        RegisterResponse,
         PlanetLocation,
         OverviewViewResponse,
         BuildingsViewResponse,
         StartBuildingActionResponse,
        Building, 
        ProductionInformationViewResponse} from "./generated/AllGenerated"

export default interface IRouterConnectivity
{
    tryLogin(credentials: UserCredentials) : Promise<LoginResponse>
    tryRegister(credentials: UserCredentials) : Promise<RegisterResponse>
    loadOverviewPage(planet: PlanetLocation) : Promise<OverviewViewResponse>
    loadBuildingsPage(planet: PlanetLocation) : Promise<BuildingsViewResponse>
    loadResourcesPage(planet: PlanetLocation) : Promise<ProductionInformationViewResponse>
    beginBuilding(planet: PlanetLocation, building: Building) : Promise<StartBuildingActionResponse>
}