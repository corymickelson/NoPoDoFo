import { IDictionary } from "./dictionary";

export interface IObject {
    length:number
    type:string
    setDictionary(dict: IDictionary): void
}

