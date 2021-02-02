import {IRouter} from "./IRouter"
import fetch from "node-fetch"
import {ChildProcess, spawn} from "child_process"
import {OnPlanetRequest} from "./OnPlanetRequest"
import {GeneralRequest} from "./GeneralRequest"
import {GeneralRequestData} from "./GeneralRequestData"
import {GeneralResponse} from "./GeneralResponse"
import {GeneralResponseData} from "./GeneralResponseData"
import {OnPlanetResponse} from "./OnPlanetResponse"
import {OnPlanetResponseData} from "./OnPlanetResponseData"
import {RnDResponseData} from "./RnDResponseData"
import {RnDResponse} from "./RnDResponse"
import {SerializableRequest} from "./SerializableRequest"
import {GENERAL_REQUEST} from "./DiscGeneralRequest"
import {ON_PLANET_REQUEST} from "./DiscOnPlanetRequest"
import { RN_D_REQUEST } from "./DiscRnDRequest"
import { SerializableResponse} from "./SerializableResponse"
import * as zmq from "zeromq"

export class RemoteZmqRouter implements IRouter
{
    constructor(remoteAddress: string, binaryPath: string)
    {
        this.remoteAddress = remoteAddress;
        this.remoteProcess = spawn(binaryPath);
        
    }
    async generalRequest(request: GeneralRequestData) : Promise<GeneralResponseData>
    {
        const resp = await this.delegateToBinary<GeneralRequest, GeneralResponse>({request: request}, GENERAL_REQUEST);
        return resp.response;
    }
    async onPlanetRequest(request: OnPlanetRequest) : Promise<OnPlanetResponseData>
    {            
        const resp = await this.delegateToBinary<OnPlanetRequest, OnPlanetResponse>(request, ON_PLANET_REQUEST);
        return resp.response;
    }

    async delegateToBinary<Request extends GeneralRequest | OnPlanetRequest,
                          Response extends GeneralResponse | OnPlanetResponse | RnDResponse>(request: Request, type: typeof GENERAL_REQUEST | typeof ON_PLANET_REQUEST | typeof RN_D_REQUEST) : Promise<Response>
    {
        const sock = new zmq.Request;
        sock.connect('tcp://localhost:5555');

        const req = {
            request: {
                type: type,
                data: request
            },
            transactionId: 0
        } as SerializableRequest;

        console.log(req);
        
        await sock.send(JSON.stringify(req));

        const [result] = await sock.receive();

        const res = JSON.parse(result.toString()) as SerializableResponse;

        console.log(res)

        return res.response.data as Response;
    }

    remoteAddress: string;
    remoteProcess: ChildProcess;
}
