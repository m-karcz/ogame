import {Reducer, AnyAction} from "redux"
import {registerRequest, loginSucceeded, loginRequest, registerSuccessful, contextUpdated, buildingsLoaded} from "./Actions"
import {EMPTY_CONTEXT_DATA, Store, LoginState, RegisterState, DEFAULT_STORE_STATE, getLoginPageState, getLoginFormState, OVERVIEW_PAGE, INGAME_PAGE, getIngamePageState, getChosenPlanet, BUILDINGS_PAGE, getEmptyContextWithChosen} from "./Store";

//type LoginFormAction = LoginSubmitAction | RegisterSubmitAction | LoginSuccessfulAction | RegisterSuccessfulAction;

const myReducer : Reducer<Store, AnyAction> = (state = DEFAULT_STORE_STATE, action) => {
  if(loginRequest.match(action))
    return {
      ...state,
      page : {
        ...getLoginPageState(state),
        loginForm: {
          ...getLoginFormState(state),
          loginState: LoginState.requested
        }
      }
    }
  else if(loginSucceeded.match(action))
    return {
      ...state,
      page : {
        type: INGAME_PAGE,
        innerPage : {
          type: OVERVIEW_PAGE
        },
        contextData: getEmptyContextWithChosen(action.payload.chosenPlanet)
      }
    }
  else if(registerRequest.match(action))
    return {
      page : {
        ...getLoginPageState(state),
        loginForm : {
          ...getLoginFormState(state),
          registerState: RegisterState.requested
        }
      }
    }
  else if(registerSuccessful.match(action))
    return {
      ...state,
      page : {
        ...getLoginPageState(state),
        loginForm : {
          ...getLoginFormState(state),
          registerState: RegisterState.successful
        }
      }
    }
  else if(contextUpdated.match(action))
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        contextData: {
          actualPlanetStorage: action.payload.storage,
          planetList: action.payload.planetList,
          chosenPlanet: getChosenPlanet(state)
        }
      }
    }
  else if(buildingsLoaded.match(action))
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        innerPage: {
          type: BUILDINGS_PAGE,
          buildings: action.payload.buildings,
          queue: action.payload.queue
        }
      }
    }
  else
    return state
}

export const combinedReducer = myReducer;