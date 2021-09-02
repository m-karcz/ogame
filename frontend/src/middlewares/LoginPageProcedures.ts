import {Middleware} from "redux"
import IRouterConnectivity from "../IRouterConnectivity"
import {loadOverviewPage, loginRequest, registerRequest, registerSuccessful, loginSucceeded, loginSucceededNew} from "../Actions"

export function getLoginMiddleware(conn: IRouterConnectivity) : Middleware
{
    return store => next => action => {
        if(loginRequest.match(action))
        {
            conn.tryLoginNewApi(action.payload).then((resp)=>{
                console.log(resp)
                if(resp.success)
                {
                    store.dispatch(loginSucceededNew());
                }
            })
        }
        else if(loginSucceeded.match(action))
        {
            store.dispatch(loadOverviewPage({planet: action.payload.chosenPlanet}));
        }
        else if(loginSucceededNew.match(action))
        {
            store.dispatch(loadOverviewPage({planet: null}));
        }
        else if(registerRequest.match(action))
        {
            conn.tryRegisterNewApi(action.payload).then(() => store.dispatch(registerSuccessful()));
        }
        next(action);
    }
}
