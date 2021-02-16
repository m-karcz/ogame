import {GeneralRequestData,
        GeneralResponseData,
        OnPlanetResponseData,
        OnPlanetRequest} from "../../build/common/generated/AllGenerated"

export interface IRouter
{
    generalRequest(request: GeneralRequestData) : Promise<GeneralResponseData>
    onPlanetRequest(request: OnPlanetRequest) : Promise<OnPlanetResponseData>;
}