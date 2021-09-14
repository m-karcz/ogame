import {ChildProcess, spawn} from "child_process"
import * as zmq from "zeromq"

export class RemoteZmqRouter
{
    constructor(remoteAddress: string, binaryPath: string)
    {
        this.remoteAddress = remoteAddress;
        this.remoteProcess = spawn(binaryPath, [], {stdio: "ignore"});
    }


    remoteAddress: string;
    remoteProcess: ChildProcess;
}
