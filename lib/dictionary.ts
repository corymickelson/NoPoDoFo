// const mod = require('bindings')('npdf')
import {__mod} from './document'
import {IObj} from "./object";

export type CoerceKeyType = 'boolean' | 'long' | 'name' | 'real'

export interface IDictionary {
    dirty: boolean
    immutable: boolean
    _instance: any

    getKey(key: string): IObj

    getKeys(): Array<string>

    getKeyAs(type: CoerceKeyType): IObj

    hasKey(key: string): boolean

    addKey(key: string, value: IObj): void

    removeKey(key: string): void

    clear(): void

    write(dest: string, cb: (e:any, v:string) => void): void
}

export class Dictionary implements IDictionary {
    dirty: boolean;
    immutable: boolean;

    getKey(key: string): IObj {
        return this._instance.getKey(key)
    }

    getKeys(): Array<string> {
        return this._instance.getKeys()
    }

    getKeyAs(type: CoerceKeyType): IObj {
        return this._instance.getKeyAs(type)
    }

    hasKey(key: string): boolean {
        return this._instance.hasKey(key)
    }

    addKey(key: string, value: IObj): void {
        this._instance.addKey(key, value._instance)
    }

    removeKey(key: string): void {
        return this._instance.removeKey(key)
    }

    clear(): void {
        this._instance.clear()
    }

    write(output: string, cb: (e:any, v: string) => void): void {
        this._instance.write(output)
    }

    _instance: any

    constructor(obj: IObj) {
        if (obj === null) {
            throw Error("Can not instantiate Dictionary without valid NPdf Object")
        }
        this._instance = new __mod.Dictionary(obj._instance)
    }

}