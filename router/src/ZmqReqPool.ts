import * as zmq from "zeromq"


export class ZmqReqPool<RequestT, ResponseT>
{
    constructor(address: string)
    {
        this.freeRequestors = []
        this.address = address;
    }
    async makeRequest(data: RequestT): Promise<ResponseT>
    {
        if(this.freeRequestors.length === 0)
        {
            let requestor = new zmq.Request;
            requestor.connect(this.address)
            this.freeRequestors.push(requestor)
        }
        let requestor = this.freeRequestors.pop();
        await requestor.send(JSON.stringify(data))
        const [result] = await requestor.receive();
        this.freeRequestors.push(requestor);
        return JSON.parse(result) as ResponseT
    }
    freeRequestors: Array<any> 
    address: string
}