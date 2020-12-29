import express from "express"
import * as Api from "./GeneralRequest"
import {makeBuildingViewRequest} from "./ConcreteRequests"
import {RemoteRouter} from "./RemoteRouter"
import session from "express-session"

const app = express();
const port = 8888;

const router = new RemoteRouter("http://127.0.0.1:8080", "vim");

app.use(session({ secret: 'no elo', cookie: { maxAge: 60000 }}));

app.get("/", (req,res) => {
	res.send("hello world")
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
		res.send(resp.planets ? resp.planets : []);
	});
});

app.post("/register", (req, res) => {
	
	router.generalRequest(Api.makeRegisterRequest("elo", "eloelo")).then((resp) => res.send(resp));
});


app.listen(port, ()=>{
	let cred: Api.UserCredentials = { login: "aaa", passcode: "bbb"};
	let loginReq: Api.LoginRequest = { credentials: cred};
	//const generalReq = Api.makeDiscriminated(loginReq, "LoginRequest")
	//console.log(generalReq);
	console.log(makeBuildingViewRequest({id: 5}, {solar: 5, galaxy:6, position:7}));
	//console.log(req);
	console.log("working on")
});
