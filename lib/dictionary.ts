import {__mod} from './document'
import { Obj} from "./object"
import {Ref} from "./reference";
import {Arr} from "./arr";

export type CoerceKeyType = 'boolean' | 'long' | 'name' | 'real'

export class Dictionary {
    dirty: boolean;
    immutable: boolean;

    getKey(key: string): Obj {
        const instance = this._instance.getKey(key)
        return new Obj(instance)
    }

    getKeys(): Array<string> {
        return this._instance.getKeys()
    }

    getKeyAs(type: CoerceKeyType, key: string): Obj {
        return this._instance.getKeyAs(type, key)
    }

    hasKey(key: string): boolean {
        return this._instance.hasKey(key)
    }

    addKey(key: string, value: Obj): void {
        this._instance.addKey(key, value._instance)
    }

    removeKey(key: string): void {
        return this._instance.removeKey(key)
    }

    clear(): void {
        this._instance.clear()
    }

    write(output: string): Promise<string> {
        return this._instance.write(output)
    }

    writeSync(output: string): void {
        this._instance.writeSync(output)
    }

    /**
     * WARNING!!! Use this only for key lookup. Modifications made to this object will not persist
     * back to the underlying pdf object
     * @returns Object
     */
    toObject(): { [key: string]: Obj } {
        const init: { [key: string]: any } = this._instance.toObject(),
            js:any= {}
        for (let prop in init) {
            js[prop] = new Obj(init[prop])
        }
        return js
    }

    _instance: any

    constructor(obj: Obj) {
        if (obj === null) {
            throw Error("Can not instantiate Dictionary without valid NPdf Object")
        }
        this._instance = new __mod.Dictionary(obj._instance)
    }

}
