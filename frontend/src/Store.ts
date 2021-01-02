export type Planet =
{
    galaxy: number,
    solar: number,
    position: number
}

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

export type LoginFields =
{
    login: string,
    passcode: string
}

export type LoginFormState = 
{
    loginState: LoginState
    registerState: RegisterState
    loginFields: LoginFields
}

export type ResourcesViewState =
{
    metal: number,
    crystal: number,
    deuter: number
}

export type ActualPlanetData = 
{
    resourcesView: ResourcesViewState
}

export type PlanetLocation =
{
    galaxy: number,
    solar: number,
    position: number
}

export const DEFAULT_LOGIN_FIELDS : LoginFields = {
    'login': "",
    "passcode": ""
}

export const DEFAULT_LOGIN_FORM_STATE : LoginFormState = {
    'loginState' : LoginState.none,
    'registerState' : RegisterState.none,
    'loginFields': DEFAULT_LOGIN_FIELDS
};

export interface Store
{
    loginForm: LoginFormState,
    planetList: Array<PlanetLocation>,
    chosenPlanet: number
    actualPlanetData: ActualPlanetData | null,
}



export const DEFAULT_STORE_STATE : Store = {
    'loginForm' : DEFAULT_LOGIN_FORM_STATE,
    'planetList': [],
    'chosenPlanet' : 0,
    'actualPlanetData' : null
}

export function getLoginFormState(store: Store) : LoginFormState
{
    return store.loginForm;
}

export function getLoginState(store: Store) : LoginState
{
    return getLoginFormState(store).loginState;
}

export function getRegisterState(store: Store) : RegisterState
{
    return getLoginFormState(store).registerState;
}

export function getLoginFields(store: Store) : LoginFields
{
    return getLoginFormState(store).loginFields;
}

export function getActualPlanetData(store: Store) : ActualPlanetData | null
{
    return store.actualPlanetData;
}

export function getResourcesViewState(store: Store) : ResourcesViewState
{
    const planetData = getActualPlanetData(store);
    if(! planetData)
    {
        throw "no elo";
    }
    return planetData!.resourcesView;
}