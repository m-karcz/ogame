import {UserCredentials} from "./GeneralRequest"
import {Discriminated, GeneralRequestUnion, GeneralResponseUnion} from "./GeneralRequest"
import {OnPlanetRequest} from "./OnPlanetRequest"

export interface IRouter
{
    generalRequest(request: Discriminated<GeneralRequestUnion>) : Promise<GeneralResponseUnion>;
    onPlanetRequest(request: OnPlanetRequest) : Promise<any>;
}