import {Buildings,
        Storage,
        PlanetLocation,
        UserCredentials,
        Researchs, 
        OnPlanetState} from "./generated/AllGenerated"


export enum LoginState
{
    none = 0,
    requested = 1,
    successful = 2
}

export enum RegisterState
{
    none = 0,
    requested = 1,
    successful = 2
}

export type LoginFormState = 
{
    loginState: LoginState
    registerState: RegisterState
    loginFields: UserCredentials
}

export const DEFAULT_LOGIN_FIELDS : UserCredentials = {
    'login': "",
    "passcode": ""
}

export const DEFAULT_LOGIN_FORM_STATE : LoginFormState = {
    'loginState' : LoginState.none,
    'registerState' : RegisterState.none,
    'loginFields': DEFAULT_LOGIN_FIELDS
};

export const OVERVIEW_PAGE = "OVERVIEW_PAGE";
export const BUILDINGS_PAGE = "BUILDINGS_PAGE";
export const RESOURCES_PAGE = "RESOURCES_PAGE";
export const DEPENDENCY_TREE_PAGE = "DEPENDENCY_TREE_PAGE";
export const LOGIN_PAGE = "LOGIN_PAGE";
export const INGAME_PAGE = "INGAME_PAGE";


export const EMPTY_ON_PLANET : OnPlanetState =
{
    planetList: [],
    storage: 
    {
        metal: 0,
        crystal: 0,
        deuter: 0,
        lastUpdatedAt: 0
    },
    buildings: {
        metalMine: 0,
        crystalMine: 0,
        deuteriumSynthesizer: 0,
        solarPlant: 0,
        metalStorage: 0,
        deuteriumTank: 0,
        crystalStorage: 0,
        fusionReactor: 0,
        terraformer: 0,
        researchLab: 0,
        roboticsFactory: 0,
        naniteFactory: 0,
        alianceDepot: 0,
        shipyard: 0,
        missileSilo: 0
    },
    buildingQueue: null,
    productionInformation: {
        production: {
            metalMineEnergyUsage: 0,
            metalMineGeneration: 0,
            metalMineIdealGeneration: 0,
            crystalMineEnergyUsage: 0,
            crystalMineGeneration: 0,
            crystalMineIdealGeneration: 0,
            deuteriumSynthesizerEnergyUsage: 0,
            deuteriumSynthesizerGeneration: 0,
            deuteriumSynthesizerIdealGeneration: 0,
            solarPlantGeneration: 0,
            solarSateliteGeneration: 0,
            fusionReactorDeuterUsage: 0,
            fusionReactorGeneration: 0,
            fusionReactorIdealGeneration: 0
        },
        percentages: {
            metalMine: 0,
            crystalMine: 0,
            deuteriumSynthesizer: 0,
            solarPlant: 0,
            solarSatelite: 0,
            fusionReactor: 0
        }
    }
}

export enum IngamePageType 
{
    Overview = "OVERVIEW_PAGE",
    Buildings = "BUILDINGS_PAGE",
    Resources = "RESOURCES_PAGE",
    DependencyTree = "DEPENDENCY_TREE_PAGE"
}

export type LoginPageState =
{
    type: typeof LOGIN_PAGE
    loginForm: LoginFormState
};

export type IngamePageState =
{
    type: typeof INGAME_PAGE
    innerPage: IngamePageType
    onPlanet: OnPlanetState
}

export type PageState = LoginPageState | IngamePageState;

export interface Store
{
    page: PageState
}

export const DEFAULT_STORE_STATE : Store = {
    page: {
        type: LOGIN_PAGE,
        loginForm: DEFAULT_LOGIN_FORM_STATE
    }
}

export function getPageState(store: Store) : PageState
{
    return store.page;
}

export function getLoginPageState(store: Store) : LoginPageState
{
    return store.page as LoginPageState;
}

export function getIngamePageState(store: Store) : IngamePageState
{
    return store.page as IngamePageState;
}

export function getLoginFormState(store: Store) : LoginFormState
{
    return getLoginPageState(store).loginForm;
}

export function getLoginState(store: Store) : LoginState
{
    return getLoginFormState(store).loginState;
}

export function getRegisterState(store: Store) : RegisterState
{
    return getLoginFormState(store).registerState;
}

export function getLoginFields(store: Store) : UserCredentials
{
    return getLoginFormState(store).loginFields;
}

export function getActualPlanetStorage(store: Store) : Storage
{
    return getIngamePageState(store).onPlanet.storage;
}

export function getChosenPlanet(store: Store) : PlanetLocation | null
{
    if(! getIngamePageState(store).onPlanet)
    {
        return null;
    }
    if(getIngamePageState(store).onPlanet.planetList.length > 0)
    {
        return getIngamePageState(store).onPlanet.planetList[0];
    }
    return null
}

export function getBuildings(store: Store) : Buildings
{
    return getIngamePageState(store).onPlanet.buildings;
}

export function getResearchs(store: Store) : Researchs
{
    return {} as unknown as Researchs;
}

export function getBuildingQueue(store: Store)
{
    return getIngamePageState(store).onPlanet.buildingQueue;
}

export function getTotalProduction(store: Store)
{
    return getIngamePageState(store).onPlanet.productionInformation;
}

export function shouldRefreshDueToDone(store: Store)
{
  if(store.page.type === INGAME_PAGE)
  {
    if(store.page.innerPage === IngamePageType.Buildings)
    {
        if(store.page.onPlanet.buildingQueue)
        {
            return store.page.onPlanet.buildingQueue.timeToFinish < 1;
        }
    }
  }
  return false;
}