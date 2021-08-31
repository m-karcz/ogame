import {UserCredentials,
         PlanetLocation,
         LoginResponseNew,
         RegisterResponseNew,
        OnPlanetResponseNew} from "./generated/AllGenerated"

export default interface IRouterConnectivity
{
    tryLoginNewApi(credentials: UserCredentials) : Promise<LoginResponseNew>
    tryRegisterNewApi(credentials: UserCredentials) : Promise<RegisterResponseNew>
    loadOnPlanet(planet: PlanetLocation | null) : Promise<OnPlanetResponseNew>
}