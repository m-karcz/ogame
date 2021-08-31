import express, { response } from "express"
import {Request as ExpressRequest} from "express"
import {OnPlanetRequestBuilder} from "./OnPlanetRequestBuilder"
import {RemoteZmqRouter} from "./RemoteZmqRouter"
import session from "express-session"
import path from "path"
import {program} from "commander"
import bodyParser from "body-parser"
import {UserCredentials,
StorageRequest,
	STORAGE_REQUEST,
        LOGIN_REQUEST,
        REGISTER_REQUEST,
        PlayerId,
        OverviewViewRequest,
        OverviewViewResponse,
        OnPlanetResponse,
        StorageResponse,
        STORAGE_RESPONSE,
         BuildingsViewRequest,
         BuildingsViewResponse,
		 ProductionInformationViewRequest,
         BUILDINGS_LIST_REQUEST,
         BUILDING_QUEUE_REQUEST,
         BUILDING_QUEUE_RESPONSE,
         BuildingsListResponse,
         BUILDINGS_LIST_RESPONSE, 
         StartBuildingActionRequest,
         StartBuildingActionResponse,
		 ExternalGeneralRequest,
		 ExternalGeneralResponse,
		 InternalGeneralResponse,
		 InternalGeneralRequest,
         BUILD_REQUEST, 
		 LOGIN_RESPONSE_NEW,
		 REGISTER_RESPONSE_NEW,
		 LoginResponseNew,
		 RegisterResponseNew,
		 INTERNAL_LOGIN_RESPONSE,
		 OnPlanetRequestNew,
		 AuthenticatedOnPlanetRequest} from "./generated/AllGenerated"
         //BUILD_REQUEST } from "../../build/common/generated/AllGenerated"
import { RouterMiddleware} from "./RouterMiddleware"
import { NewRouterConnectivity, NewRouterMiddleware } from "./NewRouterMiddleware"

const argv = program.option("--binary <path>").parse(process.argv);

//console.log(argv)

if(! argv["binary"])
{
	throw "missing binary";
}

const app = express();
const port = 8888;

const router = new RemoteZmqRouter("http://127.0.0.1:8080", argv["binary"] as string);

const routerMiddleware = new RouterMiddleware(router);

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

//const newMiddleware = new NewRouterMiddleware(newConnectivity)


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

app.post("/game/buildings", async function(req: TypedRequest<BuildingsViewRequest>, res)
{
	const resp = await routerMiddleware.buildingsView(req.session.playerId!, req.body);
	res.send(resp);
});

app.post("/game/startBuilding", async function(req: TypedRequest<StartBuildingActionRequest>, res)
{
	const resp = await routerMiddleware.startBuilding(req.session.playerId!, req.body);
	res.send(resp);
})

app.post("/game/overview", async function(req : TypedRequest<OverviewViewRequest>, res)
{
	const resp = await routerMiddleware.overview(req.session.playerId!, req.body);
	res.send(resp);
});

app.post("/game/resources", async function(req : TypedRequest<ProductionInformationViewRequest>, res)
{
	const resp = await routerMiddleware.queryProduction(req.session.playerId!, req.body);
	res.send(resp);
})

app.post("/login", (req : TypedRequest<UserCredentials>, res) => {
	console.log(req.session)
	const pass = {
		credentials: req.body
	}
	router.generalRequest({type: LOGIN_REQUEST, data: pass}).then(function(resp: any)
	{
		console.log(resp)

		resp = resp.data;
		if(resp.playerId)
		{
			req.session.authenticated = true;
			req.session.playerId = resp.playerId
		}
		if(!resp.planets)
		{
			res.send(null)
			return
		}
		router.onPlanetRequest(new OnPlanetRequestBuilder(resp.playerId, resp.planets[0]).addQuery({type: STORAGE_REQUEST, data:{}}).msg).then((storageResp) => {
			const finalResp = {
				planets: resp.planets,
				firstPlanetStorage: storageResp
			}
			console.log(storageResp);
			res.send(finalResp)
		})
	});
});

app.post("/register", (req, res) => {
	const pass = {
		credentials: req.body as UserCredentials
	}
	router.generalRequest({type: REGISTER_REQUEST, data: pass}).then((resp) => res.send(resp.data));
});


app.listen(port, ()=>{
	//let cred: Api.UserCredentials = { login: "aaa", passcode: "bbb"};
	//let loginReq: Api.LoginRequest = { credentials: cred};
	//console.log(makeBuildingViewRequest({id: 5}, {solar: 5, galaxy:6, position:7}));
	console.log("working on")
});
