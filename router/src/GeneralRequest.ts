import {PlayerId, PlanetLocation,makeDiscriminated} from "./CommonTypes";

export type UserCredentials = 
{
    login: string,
    passcode: string
}

export type RegisterRequest =
{
    credentials: UserCredentials
}

export type RegisterResponse =
{
    status: string
}

export type LoginRequest =
{
    credentials: UserCredentials
}

export type LoginResponse =
{
    playerId?: PlayerId,
    planets: Array<PlanetLocation>
}

export type Discriminated<T> =
{
    data: T,
    type: string
}

export type GeneralRequestUnion = LoginRequest | RegisterRequest;
export type GeneralResponseUnion = LoginResponse | RegisterResponse;

export function makeLoginRequest(login: string, passcode: string) : Discriminated<LoginRequest>
{
    return makeDiscriminated<LoginRequest>({credentials : {login: login, passcode: passcode}}, "LoginRequest");
}

export function makeRegisterRequest(login: string, passcode: string) : Discriminated<RegisterRequest>
{
    return makeDiscriminated<RegisterRequest>({credentials : {login: login, passcode: passcode}}, "RegisterRequest");
}