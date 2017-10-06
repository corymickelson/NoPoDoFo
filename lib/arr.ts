import { IObj } from "./object";

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
        throw new Error("Method not implemented.");
    }
    indexOf(key: string): number {
        throw new Error("Method not implemented.");
    }
    write(output: string): void {
        throw new Error("Method not implemented.");
    }
    at(index:number): IObj {
        return this._instance.getIndex(index)
    }
}
