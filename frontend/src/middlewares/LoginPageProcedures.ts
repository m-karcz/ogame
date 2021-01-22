import {Middleware} from "redux"
import IRouterConnectivity from "../IRouterConnectivity"
import {loadOverviewPage, loginRequest, registerRequest, registerSuccessful, loginSucceeded} from "../Actions"
import { getChosenPlanet } from "../Store"

export function getLoginMiddleware(conn: IRouterConnectivity) : Middleware
{
    return store => next => action => {
        if(loginRequest.match(action))
        {
            conn.tryLogin(action.payload).then((resp)=>{
                store.dispatch(loginSucceeded({chosenPlanet: resp.planets[0]}))});
        }
        else if(loginSucceeded.match(action))
        {
            store.dispatch(loadOverviewPage({planet: action.payload.chosenPlanet}));
        }
        else if(registerRequest.match(action))
        {
            conn.tryRegister(action.payload).then((resp) => store.dispatch(registerSuccessful()));
        }
        next(action);
    }
}
