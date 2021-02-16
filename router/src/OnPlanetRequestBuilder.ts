import {OnPlanetQuery,
        OnPlanetAction,
        PlayerId,
        PlanetLocation,
        OnPlanetRequest} from "../../build/common/generated/AllGenerated"


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

    msg: OnPlanetRequest
}