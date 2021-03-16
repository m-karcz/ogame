import {Middleware} from "redux"
import IRouterConnectivity from "../IRouterConnectivity"
import {resourcesLoaded, loadResourcesPage, loadOverviewPage, loadBuildingsPage, buildingsLoaded, overviewLoaded, startBuilding, getContextUpdated, loadDependencyTreePage, dependencyTreeLoaded, refreshPage, secondElapsed} from "../Actions"
import {DEPENDENCY_TREE_PAGE, getChosenPlanet, getIngamePageState, OVERVIEW_PAGE, RESOURCES_PAGE, BUILDINGS_PAGE, Store, shouldRefreshDueToDone} from "../Store"
import {GeneralContext,
        BuildingsViewResponse,
        PlanetLocation,
        ProductionInformationViewResponse } from "../generated/AllGenerated"


function chooseRefreshAction(store: Store, planet: PlanetLocation | null = null)
{
    if(planet === null)
    {
        planet = getChosenPlanet(store);
    }
    const payload = {planet: planet};
    switch(getIngamePageState(store).innerPage.type)
    {
        case OVERVIEW_PAGE:
            return loadOverviewPage(payload);
        case BUILDINGS_PAGE:
            return loadBuildingsPage(payload);
        case RESOURCES_PAGE:
            return loadResourcesPage(payload);
        case DEPENDENCY_TREE_PAGE:
            return loadDependencyTreePage(payload);
    }
}

export function getIngameMiddleware(conn: IRouterConnectivity) : Middleware
{
    return store => next => action => {
        const {dispatch} = store;
        const updateContext = (msg: {context: GeneralContext}) => dispatch(getContextUpdated(msg));
        const updateBuildings = (msg: BuildingsViewResponse) => dispatch(buildingsLoaded({queue: msg.buildingQueue.queue}));
        const updateResources = (msg: ProductionInformationViewResponse) => dispatch(resourcesLoaded(msg.productionInformation))
        const getPlanet = () => getChosenPlanet(store.getState());
        if(loadOverviewPage.match(action))
        {
            conn.loadOverviewPage(action.payload.planet).then(resp => {updateContext(resp);
                                                                       dispatch(overviewLoaded());});
        }
        else if(loadBuildingsPage.match(action))
        {
            conn.loadBuildingsPage(getPlanet()).then(resp => {updateContext(resp);
                                                              updateBuildings(resp);});
        }
        else if(startBuilding.match(action))
        {
            conn.beginBuilding(getPlanet(), action.payload.building).then(resp => {updateContext(resp.response);
                                                                                   updateBuildings(resp.response)});
        }
        else if(loadResourcesPage.match(action))
        {
            conn.loadResourcesPage(getPlanet()).then(resp => {updateContext(resp);
                                                              updateResources(resp);});

        }
        else if(loadDependencyTreePage.match(action))
        {
            conn.refreshContext(getPlanet()).then(resp => {updateContext(resp);
                                                           dependencyTreeLoaded({})});
        }
        else if(refreshPage.match(action))
        {
            dispatch(chooseRefreshAction(store.getState()));
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
