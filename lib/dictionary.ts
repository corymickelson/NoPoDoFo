// const mod = require('bindings')('npdf')
import {__mod} from './document'
import {IObj, Obj} from "./object";

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

    write(dest: string, cb: (e:Error, v:string) => void): void

    writeSync(dest:string):void
    toObject(): {[key:string]: IObj}
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
        this._instance.write(output, cb)
    }
    writeSync(output:string):void {
        this._instance.writeSync(output)
    }

    toObject(): {[key:string]: IObj} {
        const init:{[key:string]: any} = this._instance.toObject()
        for(let prop in init) {
            init[prop] = new Obj(init[prop])
        }
        return init;
    }

    _instance: any

    constructor(obj: IObj) {
        if (obj === null) {
            throw Error("Can not instantiate Dictionary without valid NPdf Object")
        }
        this._instance = new __mod.Dictionary(obj._instance)
    }

}