import { IObj, Obj } from "./object";

export interface IArr {
    dirty: boolean
    contains(key: string): boolean
    indexOf(key: string): number
    write(output: string): void
    at(index:number): IObj
}
export class Arr implements IArr {
    dirty: boolean;

    constructor(public _instance: any) { }

    contains(key: string): boolean {
        return this._instance.contains(key)
    }
    indexOf(key: string): number {
        return this._instance.indexOf(key)
    }
    write(output: string): void {
        this._instance.write(output)
    }
    at(index:number): IObj {
        const item = this._instance.getIndex(index)
        return new Obj(item)
    }
}
