import { ZmqReqPool } from "./ZmqReqPool"

import {AuthenticatedOnPlanetRequest, InternalGeneralRequest,
        InternalGeneralResponse,
        OnPlanetResponseNew} from "./generated/AllGenerated"

export class NewRouterConnectivity
{
    constructor(generalAddress: string, onPlanetAddress: string)
    {
        this.generalReqPool = new ZmqReqPool(generalAddress);
        this.onPlanetPool = new ZmqReqPool(onPlanetAddress);
    }
    onPlanetPool: ZmqReqPool<AuthenticatedOnPlanetRequest, OnPlanetResponseNew>
    generalReqPool: ZmqReqPool<InternalGeneralRequest, InternalGeneralResponse>
}