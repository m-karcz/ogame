import * as General from "./GeneralRequest";
import * as OnPlanet from "./OnPlanetRequest";
import {PlayerId, PlanetLocation, Discriminated, makeDiscriminated} from "./CommonTypes";

export function makeBuildingViewRequest(player: PlayerId, planet: PlanetLocation) : OnPlanet.OnPlanetRequest
{
    let req = new OnPlanet.OnPlanetRequest(player, planet);

    req.addQuery(makeDiscriminated({}, "BuildingsListRequest"));
    req.addQuery(makeDiscriminated({}, "BuildingQueueRequest"));
    req.addQuery(makeDiscriminated({}, "StorageRequest"));

    return req;
}