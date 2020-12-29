import {UserCredentials} from "./GeneralRequest"
import {Discriminated, GeneralRequestUnion, GeneralResponseUnion} from "./GeneralRequest"

export interface IRouter
{
    generalRequest(request: Discriminated<GeneralRequestUnion>) : Promise<GeneralResponseUnion>;
}