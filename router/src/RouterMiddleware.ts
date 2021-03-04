import { OnPlanetRequestBuilder } from "./OnPlanetRequestBuilder"
import {IRouter} from "./IRouter"
import { BUILDINGS_LIST_RESPONSE,
         BUILDING_QUEUE_RESPONSE,
         BUILD_REQUEST,
         STORAGE_REQUEST,
         STORAGE_RESPONSE,
         BUILDINGS_LIST_REQUEST,
         BUILDING_QUEUE_REQUEST,
         PRODUCTION_PERCENTAGES_REQUEST,
         PRODUCTION_INFORMATION_REQUEST,
         PRODUCTION_PERCENTAGES_RESPONSE,
         PRODUCTION_INFORMATION_RESPONSE,
         ProductionPercentagesResponse,
         ProductionInformationResponse,
         ProductionInformationViewRequest,
         ProductionInformationViewResponse,
         BuildingsViewRequest,
         BuildingsViewResponse,
         OnPlanetResponseData,
        StorageResponse,
         BuildingsListResponse,
         BuildingQueueResponse,
        StartBuildingActionRequest,
        StartBuildingActionResponse,
        PlayerId,
        OverviewViewRequest,
        PlanetLocation,
        GeneralContext,
        OverviewViewResponse } from "./generated/AllGenerated"


function pick<T>(typename: string)
{
	return function(resp: OnPlanetResponseData) : T{
		const elem = resp.find((elem) => elem.type === typename);
		if(elem === undefined)
		{
			throw new Error("no elo");
		}
		return elem.data! as unknown as T;
	}
}

function pickContext(resp: OnPlanetResponseData, planet: PlanetLocation) : GeneralContext
{
    return {
        storage: pick<StorageResponse>(STORAGE_RESPONSE)(resp).storage,
        planetList: [planet]
    }
}

export class RouterMiddleware
{
    constructor(backend: IRouter)
    {
        this.backend = backend;
    }

    async buildingsView(playerId: PlayerId, req: BuildingsViewRequest) : Promise<BuildingsViewResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet)
                       .addQuery({type: STORAGE_REQUEST, data:{}})
                       .addQuery({type: BUILDINGS_LIST_REQUEST, data:{}})
                       .addQuery({type: BUILDING_QUEUE_REQUEST, data:{}}).msg
        
        const resp = await this.backend.onPlanetRequest(onPlanet);

        return {
            context: pickContext(resp, req.planet),
            buildingQueue: pick<BuildingQueueResponse>(BUILDING_QUEUE_RESPONSE)(resp),
            buildings: pick<BuildingsListResponse>(BUILDINGS_LIST_RESPONSE)(resp).buildings
        }
    }

    async startBuilding(playerId: PlayerId, req: StartBuildingActionRequest) : Promise<StartBuildingActionResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet, {type: BUILD_REQUEST, data: {buildingName: req.building}})
                       .addQuery({type: STORAGE_REQUEST, data:{}})
                       .addQuery({type: BUILDINGS_LIST_REQUEST, data:{}})
                       .addQuery({type: BUILDING_QUEUE_REQUEST, data:{}}).msg
        const resp = await this.backend.onPlanetRequest(onPlanet);
        return {
            response: {
                context: pickContext(resp, req.planet),
                buildingQueue: pick<BuildingQueueResponse>(BUILDING_QUEUE_RESPONSE)(resp),
                buildings: pick<BuildingsListResponse>(BUILDINGS_LIST_RESPONSE)(resp).buildings
            },
            status: "ok"
        }
    }
    async overview(playerId: PlayerId, req: OverviewViewRequest) : Promise<OverviewViewResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet).addQuery({type: STORAGE_REQUEST, data:{}}).msg;
        const resp = await this.backend.onPlanetRequest(onPlanet);
		return {
			context: pickContext(resp, req.planet)
		}
    }
    async queryProduction(playerId: PlayerId, req: ProductionInformationViewRequest) : Promise<ProductionInformationViewResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet)
                        .addQuery({type: STORAGE_REQUEST, data:{}})
                        .addQuery({type: BUILDINGS_LIST_REQUEST, data:{}})
                        .addQuery({type: PRODUCTION_INFORMATION_REQUEST, data:{}})
                        .addQuery({type: PRODUCTION_PERCENTAGES_REQUEST, data:{}}).msg;

        const resp = await this.backend.onPlanetRequest(onPlanet);

        return {
            context: pickContext(resp, req.planet),
            productionInformation: {
                buildings: pick<BuildingsListResponse>(BUILDINGS_LIST_RESPONSE)(resp).buildings,
                percentages: pick<ProductionPercentagesResponse>(PRODUCTION_PERCENTAGES_RESPONSE)(resp).percentages,
                production: pick<ProductionInformationResponse>(PRODUCTION_INFORMATION_RESPONSE)(resp).production
            }
        }
    }
    backend: IRouter
}