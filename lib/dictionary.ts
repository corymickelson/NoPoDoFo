const mod = require('bindings')('npdf')
import { IObject } from "./object";

export type CoerceKeyType = 'boolean' | 'long' | 'name' | 'real'
export interface IDictionary {
    dirty: boolean
    immutable: boolean

    getKey(key: string): IObject
    getKeys(): Array<IObject>
    getKeyAs(): IObject
    hasKey(key: string): boolean
    addKey(key: string, value: IObject): void
    removeKey(key: string): void
    clear(): void
    write():void
}

export class Dictionary {
    private _instance:any
    constructor(obj: IObject) {
        if(obj === null) {
            throw Error("Can not instantiate Dictionary without valid NPdf Object")
        }
        this._instance = new mod.Dictionary(obj)
    }
}