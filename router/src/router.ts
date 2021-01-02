import express from "express"
import * as Api from "./GeneralRequest"
import {OnPlanetRequest, StorageRequest} from "./OnPlanetRequest"
import {makeBuildingViewRequest} from "./ConcreteRequests"
import {RemoteRouter} from "./RemoteRouter"
import session from "express-session"
import path from "path"
import {program} from "commander"
import { makeDiscriminated } from "./CommonTypes"

const argv = program.option("--binary <path>").parse(process.argv);

console.log(argv)

if(! argv["binary"])
{
	throw "missing binary";
}

const app = express();
const port = 8888;

const router = new RemoteRouter("http://127.0.0.1:8080", argv["binary"] as string);

app.use(session({ secret: 'no elo', cookie: { maxAge: 60000 }}));

app.use(express.static(path.join(__dirname, '/../../frontend/build')));
app.get('*', (req,res) =>{
    res.sendFile(path.join(__dirname+'/../../frontend/build/index.html'))
});

declare module "express-session"{
	interface Session
	{
		authenticated?: boolean
		playerId?: number
	}
}

app.use("/", function(req, res, next){
	if(req.session.authenticated === undefined)
	{
		req.session.authenticated = false;
		req.session.playerId = undefined;
	}
	next();
});

app.use("/game", function(req, res, next){
	if(req.session.authenticated)
	{
		next()
		return
	}
	res.send("unauthenticated");
});

app.post("/game/buildings", function(req, res)
{
	res.send("works");
});

app.post("/login", (req, res) => {
	console.log(req.session)
	router.generalRequest(Api.makeLoginRequest("elo", "eloelo")).then(function(resp: any)
	{
		console.log(resp)
		if(resp.playerId)
		{
			req.session.authenticated = true;
			req.session.playerId = resp.playerId;
		}
		if(!resp.planets)
		{
			res.send(null)
			return
		}
		router.onPlanetRequest(new OnPlanetRequest(resp.playerId, resp.planets[0]).addQuery(makeDiscriminated<StorageRequest>({}, "StorageRequest"))).then((storageResp) => {
			const finalResp = {
				planets: resp.planets,
				firstPlanetStorage: storageResp
			}
			res.send(finalResp)
		})
	});
});

app.post("/register", (req, res) => {
	
	router.generalRequest(Api.makeRegisterRequest("elo", "eloelo")).then((resp) => res.send(resp));
});


app.listen(port, ()=>{
	let cred: Api.UserCredentials = { login: "aaa", passcode: "bbb"};
	let loginReq: Api.LoginRequest = { credentials: cred};
	console.log(makeBuildingViewRequest({id: 5}, {solar: 5, galaxy:6, position:7}));
	console.log("working on")
});
