import IRouterConnectivity from "./IRouterConnectivity"
import {UserCredentials,
         PlanetLocation,
        LoginResponseNew,
        LOGIN_REQUEST,
        ExternalGeneralRequest,
        OnPlanetRequestNew,
        RegisterResponseNew,
        OnPlanetResponseNew,
        REGISTER_REQUEST, 
        ExternalGeneralResponse} from "./generated/AllGenerated"


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
    async tryLoginNewApi(credentials: UserCredentials) : Promise<LoginResponseNew>
    {
        const resp = await fetchAs<ExternalGeneralResponse>("/api2/general", {data: {type: LOGIN_REQUEST, data: {credentials: credentials} }} as ExternalGeneralRequest)
        return resp.data.data as LoginResponseNew;
    }
    async tryRegisterNewApi(credentials: UserCredentials) : Promise<RegisterResponseNew>
    {
        const resp = await fetchAs<ExternalGeneralResponse>("/api2/general", {data: {type: REGISTER_REQUEST, data: {credentials: credentials}}} as ExternalGeneralRequest);
        return resp.data.data as RegisterResponseNew;
    }
    loadOnPlanet(planet : PlanetLocation | null, action: OnPlanetRequestNew["action"] | null = null) : Promise<OnPlanetResponseNew>
    {
        return fetchAs<OnPlanetResponseNew>("/game/api2", /*makeDefaultOnPlanetRequest()*/
                {
                    planet: planet,
                    action: action,
                    requestType: 0
                } as OnPlanetRequestNew);
    }
}