import {OnPlanetQuery,
        OnPlanetAction,
        PlayerId,
        PlanetLocation,
        OnPlanetRequest,
        STORAGE_REQUEST,
        BUILDINGS_LIST_REQUEST} from "./generated/AllGenerated"


export class OnPlanetRequestBuilder
{
    constructor(player: PlayerId, planet: PlanetLocation, action: OnPlanetAction | null = null)
    {
        this.msg = {
            playerId : player,
            planet : planet,
            action : action,
            queries : []
        }
    }
    

    addQuery(query: OnPlanetQuery) : OnPlanetRequestBuilder
    {
        this.msg.queries.push(query)
        return this;
    }

    addContext() : OnPlanetRequestBuilder
    {
        this.msg.queries.push({type: STORAGE_REQUEST, data: {}});
        this.msg.queries.push({type: BUILDINGS_LIST_REQUEST, data: {}});
        return this;
    }

    msg: OnPlanetRequest
}