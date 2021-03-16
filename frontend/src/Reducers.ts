import {Reducer, AnyAction} from "redux"
import {overviewLoaded, registerRequest, loginSucceeded, loginRequest, registerSuccessful, contextUpdated, buildingsLoaded, resourcesLoaded, dependencyTreeLoaded, secondElapsed} from "./Actions"
import {Store, LoginState, RegisterState, DEFAULT_STORE_STATE, getLoginPageState, getLoginFormState, OVERVIEW_PAGE, INGAME_PAGE, getIngamePageState, getChosenPlanet, BUILDINGS_PAGE, getEmptyContextWithChosen, RESOURCES_PAGE, DEPENDENCY_TREE_PAGE, getBuildingQueue} from "./Store";

//type LoginFormAction = LoginSubmitAction | RegisterSubmitAction | LoginSuccessfulAction | RegisterSuccessfulAction;

function reduceIfInitialized(queue: ReturnType<typeof getBuildingQueue>)
{
  if(queue)
  {
    return {
      ...queue,
      timeToFinish: queue.timeToFinish - 1,
    }
  }
  return queue;
}


function reduceSecond(state: Store)
{
  if(state.page.type === INGAME_PAGE)
  {
    if(state.page.innerPage.type === BUILDINGS_PAGE)
    {
      return {
        ...state,
        page : {
          ...getIngamePageState(state),
          innerPage: {
            ...getIngamePageState(state).innerPage,
            queue: reduceIfInitialized(getBuildingQueue(state))
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
  else if(overviewLoaded.match(action))
  {
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        innerPage : {
          type: OVERVIEW_PAGE
        },
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
  else if(contextUpdated.match(action))
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        contextData: {
          actualPlanetStorage: action.payload.storage,
          planetList: action.payload.planetList,
          chosenPlanet: getChosenPlanet(state),
          buildings: action.payload.buildings,
          researchs: action.payload.researchs
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
          queue: action.payload.queue
        }
      }
    }
  else if(resourcesLoaded.match(action))
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        innerPage : {
          type: RESOURCES_PAGE,
          production: action.payload
        }
      }
    }
  else if(dependencyTreeLoaded.match(action))
    return {
      ...state,
      page : {
        ...getIngamePageState(state),
        innerPage : {
          type: DEPENDENCY_TREE_PAGE
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