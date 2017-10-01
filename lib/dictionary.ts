const mod = require('bindings')('npdf')
import { IPDObject } from "./object";

export type CoerceKeyType = 'boolean' | 'long' | 'name' | 'real'
export interface IDictionary {
    dirty: boolean
    immutable: boolean

    getKey(key: string): IPDObject
    getKeys(): Array<IPDObject>
    getKeyAs(): IPDObject
    hasKey(key: string): boolean
    addKey(key: string, value: IPDObject): void
    removeKey(key: string): void
    clear(): void
    write():void
}

export class Dictionary {
    private _instance:any
    constructor(obj: IPDObject) {
        if(obj === null) {
            throw Error("Can not instantiate Dictionary without valid NPdf Object")
        }
        this._instance = new mod.Dictionary(obj)
    }
}