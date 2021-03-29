import {RouterMiddleware} from "./RouterMiddleware"
import IRouterConnectivity from "./IRouterConnectivity"
import { PlanetLocation,
         OverviewViewResponse,
         OverviewViewRequest,
         BuildingsViewRequest,
         BuildingsViewResponse,
         StartBuildingActionRequest,
         StartBuildingActionResponse,
         GeneralRequestData,
         GeneralResponseData,
         OnPlanetResponseData,
         OnPlanetRequest,
         LoginResponse,
         RefreshContextResponse,
         RefreshContextRequest,
         SerializableRequest,
         SerializableResponse,
         RegisterResponse,
         UserCredentials,
         GENERAL_REQUEST,
         LOGIN_REQUEST,
         LOGIN_RESPONSE,
         PlayerId,
         Building,
         REGISTER_REQUEST,
         ProductionInformationViewResponse,
         ProductionInformationViewRequest,
         ON_PLANET_REQUEST} from "./generated/AllGenerated"

type GeneralRequestDataKey = GeneralRequestData["type"];
type GeneralRequestDataData = GeneralRequestData["data"];

function asGeneralRequest(payload: GeneralRequestDataData, type: GeneralRequestDataKey) : GeneralRequestData
{
    return {
        type: type,
        data: payload
    };
}

interface WasmModule
{
    _malloc(ptr: number) : number
    _free(ptr: number) : void
    processRequest(str: number) : number
    forwardTime(duration: number) : void
    clearDb() : void
    writeAsciiToMemory(str: string, mem: number) : void
    FileSystem : any
    AsciiToString(mem: number) : string
}

declare function makeBackend() : Promise<WasmModule>

declare global
{
    interface Window {
        __forwardTime : (duration: number)=>void
        __clearDb : ()=>void
    }
}

class WasmBackend
{
    constructor()
    {
        this.module = makeBackend()
        this.module.then((module)=>{
            window.__forwardTime = (duration: number)=>module.forwardTime(duration);
            window.__clearDb = ()=>module.clearDb();
        })
    }
    async generalRequest(request: GeneralRequestData) : Promise<GeneralResponseData>
    {
        const result = await this.processRequest({request: {
            data: {
                request: request
            },
            type: GENERAL_REQUEST
        },
        transactionId : 0
        });
        return result.response.data.response as GeneralResponseData;
    }
    async onPlanetRequest(request: OnPlanetRequest) : Promise<OnPlanetResponseData>
    {
        const result = await this.processRequest({
            request: {
                data: request,
                type: ON_PLANET_REQUEST
            },
            transactionId : 0
        });
        return result.response.data.response as OnPlanetResponseData;
    }
    async processRequest(request: SerializableRequest) : Promise<SerializableResponse>
    {
        const module = await this.module;
        const inputStr = JSON.stringify(request);
        const inputMem = module._malloc(inputStr.length + 1);
        module.writeAsciiToMemory(inputStr, inputMem);
        const outputMem = module.processRequest(inputMem);
        const outputStr = module.AsciiToString(outputMem);
        module._free(outputMem);
        await new Promise<void>(function(resolve){
            module.FileSystem.syncfs(false, function(err: any){
                console.log(err);
                resolve();
            })
        });
        return JSON.parse(outputStr) as SerializableResponse;
    }
    module: Promise<WasmModule>
}

const backend = new WasmBackend();
const middleware = new RouterMiddleware(backend);

export default class RouterConnectivity implements IRouterConnectivity
{
    async tryLogin(credentials: UserCredentials) : Promise<LoginResponse>
    {
        const resp = await backend.generalRequest(asGeneralRequest({credentials: credentials}, LOGIN_REQUEST));
        if(resp.type === LOGIN_RESPONSE)
        {
            this.playerId = resp.data.playerId;
        }
        return resp.data as LoginResponse;
    }
    async tryRegister(credentials: UserCredentials) : Promise<RegisterResponse>
    {
        const resp = await backend.generalRequest(asGeneralRequest({credentials: credentials}, REGISTER_REQUEST));
        return resp.data as RegisterResponse;
    }
    loadOverviewPage(planet: PlanetLocation) : Promise<OverviewViewResponse>
    {
        return middleware.overview(this.playerId!, {planet: planet} as OverviewViewRequest);
    }
    loadBuildingsPage(planet: PlanetLocation) : Promise<BuildingsViewResponse>
    {
        return middleware.buildingsView(this.playerId!, {planet: planet} as BuildingsViewRequest);
    }
    beginBuilding(planet: PlanetLocation, building: Building) : Promise<StartBuildingActionResponse>
    {
        return middleware.startBuilding(this.playerId!, {planet: planet, building: building} as StartBuildingActionRequest);
    }
    loadResourcesPage(planet: PlanetLocation) : Promise<ProductionInformationViewResponse>
    {
        return middleware.queryProduction(this.playerId!, {planet: planet} as ProductionInformationViewRequest);
    }
    refreshContext(planet: PlanetLocation) : Promise<RefreshContextResponse>
    {
        return middleware.refreshContext(this.playerId!, {planet: planet} as RefreshContextRequest);
    }
    playerId : PlayerId | null = null;
}

