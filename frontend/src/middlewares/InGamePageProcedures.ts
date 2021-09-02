import {Middleware} from "redux"
import IRouterConnectivity from "../IRouterConnectivity"
import {loadResourcesPage, loadOverviewPage, loadBuildingsPage, pageChanged, startBuilding, loadDependencyTreePage, refreshPage, secondElapsed, onPlanetResponseLoaded} from "../Actions"
import {getIngamePageState, getChosenPlanet, shouldRefreshDueToDone, IngamePageType} from "../Store"
import { OnPlanetRequestNew } from "../generated/OnPlanetRequestNew"
import { BUILD_REQUEST } from "../generated/DiscBuildRequest"


export function getIngameMiddleware(conn: IRouterConnectivity) : Middleware
{
    return store => next => action => {
        const {dispatch} = store;
        const loadWithRedirection = (nextPage: IngamePageType, act: OnPlanetRequestNew["action"] | null = null) => {
            conn.loadOnPlanet(getChosenPlanet(store.getState()), act).then(resp => {dispatch(onPlanetResponseLoaded(resp.response));
                                                                   dispatch(pageChanged(nextPage));});
        }
        if(loadOverviewPage.match(action))
        {
            loadWithRedirection(IngamePageType.Overview);
        }
        else if(loadBuildingsPage.match(action))
        {
            loadWithRedirection(IngamePageType.Buildings);
        }
        else if(startBuilding.match(action))
        {
            loadWithRedirection(IngamePageType.Buildings,
                                {
                                    type: BUILD_REQUEST,
                                    data: {
                                        buildingName: action.payload.building
                                    }
                                });
        }
        else if(loadResourcesPage.match(action))
        {
            loadWithRedirection(IngamePageType.Resources);
        }
        else if(loadDependencyTreePage.match(action))
        {
            loadWithRedirection(IngamePageType.DependencyTree);
        }
        else if(refreshPage.match(action))
        {
            loadWithRedirection(getIngamePageState(store.getState()).innerPage);
        }
        else if(secondElapsed.match(action))
        {
            if(shouldRefreshDueToDone(store.getState()))
            {
                dispatch(refreshPage());
            }
        }

        next(action);
    }

}
