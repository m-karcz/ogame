import { OnPlanetRequestBuilder } from "./OnPlanetRequestBuilder"
import {IRouter} from "./IRouter"
import { BUILDINGS_LIST_RESPONSE,
         BUILDING_QUEUE_RESPONSE,
         BUILD_REQUEST,
         STORAGE_RESPONSE,
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
         RefreshContextRequest,
         RefreshContextResponse,
        StartBuildingActionRequest,
        StartBuildingActionResponse,
        PlayerId,
        OverviewViewRequest,
        PlanetLocation,
        Researchs,
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

const NO_RESEARCHS : Researchs =
{
    energyTechnology: 0,
    computerTechnology: 0,
    espionageTechnology: 0,
    laserTechnology: 0,
    hyperspaceDrive: 0,
    hyperspaceTechnology: 0,
    ionTechnology: 0,
    intergalacticResearchNetwork: 0,
    armourTechnology: 0,
    shieldingTechnology: 0,
    combustionDrive: 0,
    impulseDrive: 0,
    gravitonTechnology: 0,
    weaponsTechnology: 0,
    plasmaTechnology: 0
}

function pickContext(resp: OnPlanetResponseData, planet: PlanetLocation) : GeneralContext
{
    return {
        storage: pick<StorageResponse>(STORAGE_RESPONSE)(resp).storage,
        buildings: pick<BuildingsListResponse>(BUILDINGS_LIST_RESPONSE)(resp).buildings,
        researchs: NO_RESEARCHS,
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
                       .addContext()
                       .addQuery({type: BUILDING_QUEUE_REQUEST, data:{}}).msg
        
        const resp = await this.backend.onPlanetRequest(onPlanet);

        return {
            context: pickContext(resp, req.planet),
            buildingQueue: pick<BuildingQueueResponse>(BUILDING_QUEUE_RESPONSE)(resp)
        }
    }

    async startBuilding(playerId: PlayerId, req: StartBuildingActionRequest) : Promise<StartBuildingActionResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet, {type: BUILD_REQUEST, data: {buildingName: req.building}})
                       .addContext()
                       .addQuery({type: BUILDING_QUEUE_REQUEST, data:{}}).msg
        const resp = await this.backend.onPlanetRequest(onPlanet);
        return {
            response: {
                context: pickContext(resp, req.planet),
                buildingQueue: pick<BuildingQueueResponse>(BUILDING_QUEUE_RESPONSE)(resp)
            },
            status: "ok"
        }
    }
    async overview(playerId: PlayerId, req: OverviewViewRequest) : Promise<OverviewViewResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet).addContext().msg;
        const resp = await this.backend.onPlanetRequest(onPlanet);
		return {
			context: pickContext(resp, req.planet)
		}
    }
    async queryProduction(playerId: PlayerId, req: ProductionInformationViewRequest) : Promise<ProductionInformationViewResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet)
                        .addContext()
                        .addQuery({type: PRODUCTION_INFORMATION_REQUEST, data:{}})
                        .addQuery({type: PRODUCTION_PERCENTAGES_REQUEST, data:{}}).msg;

        const resp = await this.backend.onPlanetRequest(onPlanet);

        return {
            context: pickContext(resp, req.planet),
            productionInformation: {
                percentages: pick<ProductionPercentagesResponse>(PRODUCTION_PERCENTAGES_RESPONSE)(resp).percentages,
                production: pick<ProductionInformationResponse>(PRODUCTION_INFORMATION_RESPONSE)(resp).production
            }
        }
    }
    async refreshContext(playerId: PlayerId, req: RefreshContextRequest) : Promise<RefreshContextResponse>
    {
        const onPlanet = new OnPlanetRequestBuilder(playerId, req.planet).addContext().msg;
        const resp = await this.backend.onPlanetRequest(onPlanet);

        return {
            context: pickContext(resp, req.planet),
        }
    }
    backend: IRouter
}