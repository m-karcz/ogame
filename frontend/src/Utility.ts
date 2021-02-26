
export function getKeys<T>(obj: T) : (keyof T)[]
{
    return Object.keys(obj).map((key)=>key as keyof T);
}