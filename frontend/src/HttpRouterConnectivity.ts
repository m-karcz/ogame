import IRouterConnectivity from "./IRouterConnectivity"
import {UserCredentials,
         PlanetLocation,
        LoginResponseNew,
        LOGIN_REQUEST,
        ExternalGeneralRequest,
        OnPlanetRequestNew,
        RegisterResponseNew,
        OnPlanetResponseNew,
        REGISTER_REQUEST } from "./generated/AllGenerated"


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

function makeDefaultOnPlanetRequest() : OnPlanetRequestNew
{
    return {
        planet : null,
        action: null,
        requestType: 0
    }
}

async function fetchAs<T>(dest: string, payload: any) : Promise<T>
{
    const resp = await fetch(dest, withPayload(payload));
    return await resp.json() as T;
}

export default class RouterConnectivity implements IRouterConnectivity
{
    tryLoginNewApi(credentials: UserCredentials) : Promise<LoginResponseNew>
    {
        return fetchAs<LoginResponseNew>("/api2/general", {data: {type: LOGIN_REQUEST, data: {credentials: credentials} }} as ExternalGeneralRequest);
    }
    tryRegisterNewApi(credentials: UserCredentials) : Promise<RegisterResponseNew>
    {
        return fetchAs<RegisterResponseNew>("/api2/general", {data: {type: REGISTER_REQUEST, data: {credentials: credentials}}} as ExternalGeneralRequest);
    }
    loadOnPlanet(planet : PlanetLocation | null) : Promise<OnPlanetResponseNew>
    {
        return fetchAs<OnPlanetResponseNew>("/game/api2", makeDefaultOnPlanetRequest());
    }
}