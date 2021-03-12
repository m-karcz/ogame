
export function getKeys<T>(obj: T) : (keyof T)[]
{
    return Object.keys(obj).map((key)=>key as keyof T);
}

export function prettyName(camelCase: string) : string
{
    const camelCaseName = camelCase[0].toUpperCase() + camelCase.substring(1);
    return camelCaseName.match(/[A-Z][^A-Z]*/g)?.join(" ")!;
}