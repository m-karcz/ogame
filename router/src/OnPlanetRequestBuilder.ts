import {OnPlanetRequest} from "./OnPlanetRequest"
import {OnPlanetQuery} from "./OnPlanetQuery"
import {OnPlanetAction} from "./OnPlanetAction"
import {PlayerId} from "./PlayerId"
import {PlanetLocation} from "./PlanetLocation"


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