import express, { response } from "express"
import {Request as ExpressRequest} from "express"
import {RemoteZmqRouter} from "./RemoteZmqRouter"
import session from "express-session"
import path from "path"
import {program} from "commander"
import bodyParser from "body-parser"
import { PlayerId,
		 ExternalGeneralRequest,
		 ExternalGeneralResponse,
		 InternalGeneralResponse,
		 InternalGeneralRequest,
		 LOGIN_RESPONSE_NEW,
		 REGISTER_RESPONSE_NEW,
		 LoginResponseNew,
		 RegisterResponseNew,
		 INTERNAL_LOGIN_RESPONSE,
		 OnPlanetRequestNew,
		 AuthenticatedOnPlanetRequest} from "./generated/AllGenerated"
import { NewRouterConnectivity } from "./NewRouterMiddleware"

const argv = program.option("--binary <path>").parse(process.argv);

if(! argv["binary"])
{
	throw "missing binary";
}

const app = express();
const port = 8888;

const router = new RemoteZmqRouter("http://127.0.0.1:8080", argv["binary"] as string);

app.use(session({ secret: 'no elo', cookie: { maxAge: 600000 }}));

app.use(express.static(path.join(__dirname, '/../../../frontend/build')));
app.get('*', (req,res) =>{
    res.sendFile(path.join(__dirname+'../../../../frontend/build/index.html'))
});

app.use(bodyParser.json())

declare module "express-session"{
	interface Session
	{
		authenticated?: boolean
		playerId: PlayerId | null
	}
}

interface TypedRequest<T> extends ExpressRequest
{
	body: T
}

const newConnectivity = new NewRouterConnectivity("tcp://127.0.0.1:3333", "tcp://127.0.0.1:1234")

app.use(function(req, res, next) {
	if(req.session.authenticated === undefined)
	{
		req.session.authenticated = false;
		req.session.playerId = null;
	}
	console.log(req.body)
	console.log(req.session)
	console.log("fixed session")
	if(req.path.startsWith("/game"))
	{
		if(!req.session.authenticated)
		{
			console.log("unauthenticated")
			res.send("unauthenticated");
		}
		else
		{
			console.log("authenticated, go on")
			next()
		}
	}
	else
	{
		next()
	}
})

function convertGeneralRequest(req: ExternalGeneralRequest) : InternalGeneralRequest
{
	return {
			data:
			{
				data: req.data.data,
				type: req.data.type
			}
	};
}


app.post("/api2/general", async (req: TypedRequest<ExternalGeneralRequest>, res) => {
	console.log("received general request");
	const internalResp = await newConnectivity.generalReqPool.makeRequest(convertGeneralRequest(req.body));
	console.log("made general request");
	const sendResp = (type: typeof LOGIN_RESPONSE_NEW | typeof REGISTER_RESPONSE_NEW, data: LoginResponseNew | RegisterResponseNew) => {
		res.send({
			data: {
				type: type,
				data: data
			}
		} as ExternalGeneralResponse)
	}
	switch(internalResp.data.type)
	{
		case INTERNAL_LOGIN_RESPONSE:
		{
			if(internalResp.data.data.playerId !== null)
			{
				req.session.authenticated = true;
				req.session.playerId = internalResp.data.data.playerId;
				sendResp(LOGIN_RESPONSE_NEW, {success: true} as LoginResponseNew);
			}
			else
			{
				sendResp(LOGIN_RESPONSE_NEW, {success: false} as LoginResponseNew);
			}
			break;
		}
		case REGISTER_RESPONSE_NEW:
		{
			sendResp(REGISTER_RESPONSE_NEW, {success: (internalResp.data.data as RegisterResponseNew).success});
			break;
		}
	}
});

app.post("/game/api2", async (req: TypedRequest<OnPlanetRequestNew>, res)=>{
	const resp = await newConnectivity.onPlanetPool.makeRequest({
		playerId: req.session.playerId,
		request: req.body
	} as AuthenticatedOnPlanetRequest);
	res.send(resp);
});

app.listen(port, ()=>{
	console.log("working on")
});
