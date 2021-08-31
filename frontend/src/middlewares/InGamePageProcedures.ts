import {Middleware} from "redux"
import IRouterConnectivity from "../IRouterConnectivity"
import {loadResourcesPage, loadOverviewPage, loadBuildingsPage, pageChanged, startBuilding, loadDependencyTreePage, refreshPage, secondElapsed, onPlanetResponseLoaded} from "../Actions"
import {getIngamePageState, shouldRefreshDueToDone, IngamePageType} from "../Store"


export function getIngameMiddleware(conn: IRouterConnectivity) : Middleware
{
    return store => next => action => {
        const {dispatch} = store;
        const loadWithRedirection = (nextPage: IngamePageType) => {
            conn.loadOnPlanet(action.payload.planet).then(resp => {dispatch(onPlanetResponseLoaded(resp.response));
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
            /*conn.beginBuilding(getPlanet(), action.payload.building).then(resp => {updateContext(resp.response);
                                                                                   updateBuildings(resp.response)});*/
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
