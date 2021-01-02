import {LoginFields, PlanetLocation, ResourcesViewState} from "./Store"


export const LOGIN_SUBMIT = "LOGIN_SUBMIT"
export const LOGIN_SUCCESSFUL = "LOGIN_SUCCESSFUL"
export const REGISTER_SUBMIT = "REGISTER_SUBMIT"
export const REGISTER_SUCCESSFUL = "REGISTER_SUCCESSFUL"
export const BUILDINGS_CLICKED = "BUILDINGS_CLICKED"

export type LoginSubmitAction = 
{
    type: typeof LOGIN_SUBMIT
    payload: LoginFields
}

export type SuccessfulLoginPayload =
{
    planetList: Array<PlanetLocation>,
    resources: ResourcesViewState
}

export type LoginSuccessfulAction =
{
    type: typeof LOGIN_SUCCESSFUL
    payload: SuccessfulLoginPayload
}

export type RegisterSubmitAction =
{
    type: typeof REGISTER_SUBMIT,
    payload: LoginFields
}

export type RegisterSuccessfulAction = 
{
    type: typeof REGISTER_SUCCESSFUL,
    payload: never
}

export type BuildingsClickedAction =
{
    type: typeof BUILDINGS_CLICKED,
    payload: never
}
