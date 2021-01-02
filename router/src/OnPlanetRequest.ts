import {PlayerId, PlanetLocation,Discriminated,makeDiscriminated} from "./CommonTypes"

type Buildings =
{
    metalMine: number,
    crystalMine: number,
    deuterExtractor: number,
    fusionReactor: number,
    robots: number,
    nanites: number,
    shipyard: number,
    metalStorage: number,
    crystalStorage: number,
    deuterStorage: number,
    researchLab: number,
    alianceDepot: number,
    missileSilo: number,
    terraformer: number
}

export type BuildingQueueEntry = 
{
    building: string,
    timeToFinish: number
}

export type BuildRequest =
{
    buildingName: string
}

export type StorageRequest = {}
export type BuildingQueueRequest = {}
export type BuildingsListRequest = {}

type Optional<T> = T | null;

type ActionType = Optional<Discriminated<BuildRequest>>;
type QueryType = Discriminated<BuildingQueueRequest | BuildingsListRequest | StorageRequest>;

export class OnPlanetRequest
{
    constructor(player: PlayerId, planet: PlanetLocation, action: ActionType = null)
    {
        this.playerId = player;
        this.planet = planet;
        this.action = action;
        this.queries = [];
    }
    
    addQuery(query: QueryType) : OnPlanetRequest
    {
        this.queries.push(query);
        return this;
    }
    action?: ActionType;
    queries: Array<QueryType>;
    playerId: PlayerId;
    planet: PlanetLocation;
}