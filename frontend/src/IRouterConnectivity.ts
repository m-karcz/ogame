import {UserCredentials,
         PlanetLocation,
         LoginResponseNew,
         RegisterResponseNew,
         OnPlanetRequestNew,
        OnPlanetResponseNew} from "./generated/AllGenerated"

export default interface IRouterConnectivity
{
    tryLoginNewApi(credentials: UserCredentials) : Promise<LoginResponseNew>
    tryRegisterNewApi(credentials: UserCredentials) : Promise<RegisterResponseNew>

    loadOnPlanet(planet: PlanetLocation | null, action: OnPlanetRequestNew["action"] | null) : Promise<OnPlanetResponseNew>
}