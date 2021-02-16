import express from "express"
import {Request as ExpressRequest} from "express"
import {OnPlanetRequestBuilder} from "./OnPlanetRequestBuilder"
//import {RemoteRouter} from "./RemoteRouter"
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
         BUILDINGS_LIST_REQUEST,
         BUILDING_QUEUE_REQUEST,
         BuildingQueueResponse,
         BUILDING_QUEUE_RESPONSE,
         BuildingsListResponse,
         BUILDINGS_LIST_RESPONSE, 
         StartBuildingActionRequest,
         StartBuildingActionResponse,
         BUILD_REQUEST } from "../../build/common/generated/AllGenerated"
import { RouterMiddleware} from "./RouterMiddleware"

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

app.use(express.static(path.join(__dirname, '/../../../../frontend/build')));
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
