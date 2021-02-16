import {Middleware} from "redux"
import IRouterConnectivity from "../IRouterConnectivity"
import {resourcesLoaded, loadResourcesPage, loadOverviewPage, loadBuildingsPage, buildingsLoaded, overviewLoaded, startBuilding, getContextUpdated} from "../Actions"
import {getChosenPlanet} from "../Store"
import {GeneralContext,
        BuildingsViewResponse } from "../../../build/common/generated/AllGenerated"

type TotalProductionViewResponse = any;

export function getIngameMiddleware(conn: IRouterConnectivity) : Middleware
{
    return store => next => action => {
        const {dispatch} = store;
        const updateContext = (msg: {context: GeneralContext}) => dispatch(getContextUpdated(msg));
        const updateBuildings = (msg: BuildingsViewResponse) => dispatch(buildingsLoaded({buildings: msg.buildings,
                                                                                          queue: msg.buildingQueue.queue}));
        const updateResources = (msg: TotalProductionViewResponse) => dispatch(resourcesLoaded(msg.production))
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
            /*conn.loadResourcesPage(getPlanet()).then(resp => {updateContext(resp);
                                                              updateResources(resp);});*/

        }

        next(action);
    }

}
