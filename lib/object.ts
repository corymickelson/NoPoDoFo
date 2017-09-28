import { IDictionary } from "./dictionary";


export interface IObject {
    length:number
    type:string
    getDictionary():IDictionary
    setDictionary(dict: IDictionary): void
}

