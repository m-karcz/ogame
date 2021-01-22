import {GeneralRequest} from "./GeneralRequest"
import {GeneralResponse} from "./GeneralResponse"
import {OnPlanetRequest} from "./OnPlanetRequest"

export interface IRouter
{
    generalRequest(request: GeneralRequest) : Promise<GeneralResponse>
    onPlanetRequest(request: OnPlanetRequest) : Promise<any>;
}