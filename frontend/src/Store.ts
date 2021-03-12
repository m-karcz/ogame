import {Buildings,
        Storage,
        PlanetLocation,
        UserCredentials,
        BuildingQueueResponse,
        ProductionInformation,
        Researchs } from "./generated/AllGenerated"

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


export type ContextData = 
{
    planetList: Array<PlanetLocation>,
    chosenPlanet: PlanetLocation,
    actualPlanetStorage: Storage,
    buildings: Buildings,
    researchs: Researchs
}

export const EMPTY_CONTEXT_DATA : ContextData = 
{
    planetList: [],
    chosenPlanet: { position: 0, galaxy: 0, solar: 0 },
    actualPlanetStorage: { metal: 0, crystal: 0, deuter: 0, lastUpdatedAt: 0},
    buildings: undefined as unknown as Buildings,
    researchs: undefined as unknown as Researchs
}

export function getEmptyContextWithChosen(chosenPlanet: PlanetLocation) : ContextData
{
    return {
        planetList: [chosenPlanet],
        chosenPlanet: chosenPlanet,
        actualPlanetStorage: {metal: 0, crystal: 0, deuter: 0, lastUpdatedAt: 0},
        buildings: undefined as unknown as Buildings,
        researchs: undefined as unknown as Researchs
    }
}

export type OverviewPageState = 
{
    type: typeof OVERVIEW_PAGE
}

export type BuildingsPageState =
{
    type: typeof BUILDINGS_PAGE
    queue: BuildingQueueResponse["queue"]
}

export type ResourcesPageState =
{
    type: typeof RESOURCES_PAGE
    production: ProductionInformation
}

export type DependencyTreePageState =
{
    type: typeof DEPENDENCY_TREE_PAGE
}

export type IngameInnerPageState = OverviewPageState | BuildingsPageState | ResourcesPageState | DependencyTreePageState;

export type LoginPageState =
{
    type: typeof LOGIN_PAGE
    loginForm: LoginFormState
};

export type IngamePageState =
{
    type: typeof INGAME_PAGE
    innerPage: IngameInnerPageState
    contextData: ContextData
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
    return getIngamePageState(store).contextData.actualPlanetStorage;
}

export function getChosenPlanet(store: Store) : PlanetLocation
{
    return getIngamePageState(store).contextData.chosenPlanet;
}

export function getBuildings(store: Store) : Buildings
{
    return getIngamePageState(store).contextData.buildings;
}

export function getResearchs(store: Store) : Researchs
{
    return getIngamePageState(store).contextData.researchs;
}

export function getBuildingQueue(store: Store)
{
    return (getIngamePageState(store).innerPage as BuildingsPageState).queue;
}

export function getTotalProduction(store: Store)
{
    return (getIngamePageState(store).innerPage as ResourcesPageState).production;
}