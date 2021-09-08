import {Reducer, AnyAction} from "redux"
import {registerRequest, loginRequest, registerSuccessful, secondElapsed, onPlanetResponseLoaded, pageChanged, loginSucceededNew} from "./Actions"
import {Store, LoginState, RegisterState, DEFAULT_STORE_STATE, getLoginPageState, getLoginFormState, INGAME_PAGE, getIngamePageState, getBuildingQueue, EMPTY_ON_PLANET, IngamePageType} from "./Store";

function reduceSecond(state: Store)
{
  console.log("tried to reduce second");
  if(state.page.type === INGAME_PAGE)
  {
    if(state.page.onPlanet.buildingQueue)
    {
      console.log("reduced second");
      return {
        ...state,
        page : {
          ...getIngamePageState(state),
          onPlanet: {
            ...getIngamePageState(state).onPlanet,
            buildingQueue: {
              ...getBuildingQueue(state)!,
              timeToFinish: getBuildingQueue(state)!.timeToFinish - 1
            }
          }
        }
      }
    }
  }
  return state;
}

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
  else if(loginSucceededNew.match(action))
  {
    return {
      ...state,
      page: {
        ...state.page,
        type: INGAME_PAGE,
        innerPage: IngamePageType.Overview,
        onPlanet: EMPTY_ON_PLANET
      }
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
  else if(pageChanged.match(action))
  {
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        innerPage : action.payload
      }
    }
  }
  else if(onPlanetResponseLoaded.match(action))
  {
    return {
      ...state,
      page: {
        ...getIngamePageState(state),
        onPlanet: action.payload
      }
    }
  }
  else if(secondElapsed.match(action))
  {
    return reduceSecond(state);
  }
  else
    return state
}

export const combinedReducer = myReducer;