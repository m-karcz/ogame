import {IRouter} from "./IRouter"
import {Discriminated, GeneralRequestUnion, GeneralResponseUnion} from "./GeneralRequest"
import fetch from "node-fetch"
import {ChildProcess, spawn} from "child_process"
import {OnPlanetRequest} from "./OnPlanetRequest"

export class RemoteRouter implements IRouter
{
    constructor(remoteAddress: string, binaryPath: string)
    {
        this.remoteAddress = remoteAddress;
        this.remoteProcess = spawn(binaryPath);
    }
    generalRequest(request: Discriminated<GeneralRequestUnion>) : Promise<GeneralResponseUnion>
    {
        return this.delegateToBinary<Discriminated<GeneralRequestUnion>, GeneralResponseUnion>(request, "/general");
    }
    onPlanetRequest(request: OnPlanetRequest) : Promise<any>
    {            
        return this.delegateToBinary<OnPlanetRequest, any>(request, "/on_planet");
    }

    async delegateToBinary<Request, Response>(request: Request, path: string) : Promise<Response>
    {
	    console.log("delegating")
        const response = await fetch(this.remoteAddress + path, {method: 'POST', body: JSON.stringify(request), headers: {'Content-Type': 'application/json'}});
        const json = await response.json();
        return await json as Response;
    }

    remoteAddress: string;
    remoteProcess: ChildProcess;
}
