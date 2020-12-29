export type PlayerId =
{
    id: number
}

export type PlanetLocation = 
{
    galaxy: number,
    solar: number,
    position: number
}

export type Discriminated<T> =
{
    data: T,
    type: string
}

export function makeDiscriminated<T>(data: T, type: string) : Discriminated<T>
{
    return {
        data: data,
        type: type
    };
}
