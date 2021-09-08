import {RouterMiddleware} from "./RouterMiddleware"
import IRouterConnectivity from "./IRouterConnectivity"
import { PlanetLocation,
         UserCredentials,
         LoginResponseNew,
         RegisterResponseNew,
         OnPlanetResponseNew,
         PlayerId,
         OnPlanetRequestNew,
         InternalGeneralRequest,
         InternalGeneralResponse,
         REGISTER_REQUEST,
         LOGIN_REQUEST,
         INTERNAL_LOGIN_RESPONSE,
         AuthenticatedOnPlanetRequest} from "./generated/AllGenerated"


interface WasmModule
{
    //_malloc(ptr: number) : number
    //_free(ptr: number) : void
    //processRequest(str: number) : number
    forwardTime(duration: number) : void
    clearDb() : void
    //writeAsciiToMemory(str: string, mem: number) : void
    FileSystem : any
    //AsciiToString(mem: number) : string
}

declare function makeBackend() : Promise<WasmModule>

declare global
{
    interface Window {
        __forwardTime : (duration: number)=>void
        __clearDb : ()=>void
        __elo : any
    }
}

function requestAsPromise(dealer: any, request: any) : Promise<any>
{
    return new Promise<any>((resolve, reject)=> {
        dealer.registerCallback((data: string)=>{
            console.log("received response " + data + " on request: " + request);
            resolve(JSON.parse(data));
        });
        dealer.sendMessage(JSON.stringify(request));
    });
}

class WasmBackend
{
    constructor()
    {
        this.module = makeBackend()
        this.module.then((module)=>{
            window.__forwardTime = (duration: number)=>module.forwardTime(duration);
            window.__clearDb = ()=>module.clearDb();
            window.__elo = this.module
            this.generalRequestDealer = (module as any).getGeneralRequestDealer();
            this.onPlanetDealer = (module as any).getOnPlanetDealer();
        })
    }
    async generalRequestNew(request: InternalGeneralRequest) : Promise<InternalGeneralResponse>
    {
        return requestAsPromise(this.generalRequestDealer, request) as Promise<InternalGeneralResponse>
    }
    async onPlanetNew(request: AuthenticatedOnPlanetRequest) : Promise<OnPlanetResponseNew>
    {
        return requestAsPromise(this.onPlanetDealer, request) as Promise<OnPlanetResponseNew>
    }
    module: Promise<WasmModule>
    generalRequestDealer: any
    onPlanetDealer: any
}

const backend = new WasmBackend();
//const middleware = new RouterMiddleware(backend);

export default class RouterConnectivity implements IRouterConnectivity
{
    async tryLoginNewApi(credentials: UserCredentials) : Promise<LoginResponseNew>
    {
        const result = await backend.generalRequestNew({
            data: {
                type: LOGIN_REQUEST,
                data: {
                    credentials: credentials
                }
            }
        });
        if(result.data.type === INTERNAL_LOGIN_RESPONSE)
        {
            this.playerId =result.data.data.playerId;
        }
        return {
            success: this.playerId !== null
        }
    }
    async tryRegisterNewApi(credentials: UserCredentials) : Promise<RegisterResponseNew>
    {
        const result = await backend.generalRequestNew({
            data: {
                type: REGISTER_REQUEST,
                data: {
                    credentials: credentials
                }
            }
        });
        return result.data.data as RegisterResponseNew;
    }
    async loadOnPlanet(planet: PlanetLocation | null, action: OnPlanetRequestNew["action"]) : Promise<OnPlanetResponseNew>
    {
        return backend.onPlanetNew({
            request: {
                planet: planet,
                action: action,
                requestType: 0
            },
            playerId: this.playerId!
        })

    }
    playerId : PlayerId | null = null;
}

