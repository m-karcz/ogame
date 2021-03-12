import { sortAndDeduplicateDiagnostics } from "typescript";
import { Requirements,
         Buildings,
         Researchs } from "./generated/AllGenerated"

export function getKeys<T>(obj: T) : (keyof T)[]
{
    return Object.keys(obj).map((key)=>key as keyof T);
}

export function prettyName(camelCase: string) : string
{
    const camelCaseName = camelCase[0].toUpperCase() + camelCase.substring(1);
    return camelCaseName.match(/[A-Z][^A-Z]*/g)?.join(" ")!;
}

export function findRequirements<T>(entity: T, requirementList: {name: T, requirements: Requirements}[]) : Requirements
{
    return requirementList.find((req)=>req.name === entity)!.requirements;
}

export function isSatisfied(requirements: Requirements, buildings: Buildings, researchs: Researchs)
{
    return requirements.buildings.every((req) => buildings[req.name] >= req.level)
           &&
           requirements.researchs.every((req) => researchs[req.name] >= req.level);
}