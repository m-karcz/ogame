import {GeneralRequestData} from "./GeneralRequestData"
import {GeneralResponseData} from "./GeneralResponseData"
import {OnPlanetResponseData} from "./OnPlanetResponseData"
import {OnPlanetRequest} from "./OnPlanetRequest"

export interface IRouter
{
    generalRequest(request: GeneralRequestData) : Promise<GeneralResponseData>
    onPlanetRequest(request: OnPlanetRequest) : Promise<OnPlanetResponseData>;
}