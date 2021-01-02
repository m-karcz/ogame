import {Reducer, combineReducers} from "redux"
import {LOGIN_SUBMIT, REGISTER_SUBMIT, REGISTER_SUCCESSFUL, LOGIN_SUCCESSFUL, LoginSubmitAction, RegisterSubmitAction, LoginSuccessfulAction, RegisterSuccessfulAction} from "./Actions"
import {Store, ActualPlanetData, DEFAULT_LOGIN_FORM_STATE, LoginFormState, LoginState, RegisterState, PlanetLocation} from "./Store";

type LoginFormAction = LoginSubmitAction | RegisterSubmitAction | LoginSuccessfulAction | RegisterSuccessfulAction;

const loginFormReducer : Reducer<LoginFormState, LoginFormAction> = (state = DEFAULT_LOGIN_FORM_STATE, action) =>
{
  switch(action.type)
  {
    case LOGIN_SUBMIT:
    {
      return {
        ...state,
        "loginState": LoginState.requested,
        "loginFields": action.payload
      }
    }
    case REGISTER_SUBMIT:
    {
      return {
        ...state,
        "registerState": RegisterState.requested,
        "loginFields": action.payload
      }
    }
    case REGISTER_SUCCESSFUL:
    {
      return {
        ...state,
        "registerState": RegisterState.successful
      }
    }
    case LOGIN_SUCCESSFUL:
    {
      return {
        ...state,
        "loginState": LoginState.successful
      }
    }
    default:
    {
      return state;
    }
  }
}

const planetListReducer : Reducer<Array<PlanetLocation>, LoginFormAction> = (state, action) => {
    switch(action.type)
    {
      case LOGIN_SUCCESSFUL:
      {
        return action.payload.planetList
      }
      default:
      {
        return state ? state : [];
      }
    }
}
const chosenPlanetReducer : Reducer<number, LoginFormAction> = (state, action) => {
    return state ? state : 0;
}
const actualPlanetDataReducer : Reducer<ActualPlanetData | null, LoginFormAction> = (state, action) => {
    switch(action.type)
    {
      case LOGIN_SUCCESSFUL:
      {
        return {
          ...state,
          "resourcesView": action.payload.resources
        }
      }
      default:
      {
         return state ? state : null;
      }
    }
}

export const combinedReducer = combineReducers<Store, LoginFormAction>({"loginForm": loginFormReducer,
                                                                        "planetList": planetListReducer,
                                                                        "chosenPlanet": chosenPlanetReducer,
                                                                        "actualPlanetData": actualPlanetDataReducer});