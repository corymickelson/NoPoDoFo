import {Obj} from "./object";


export class Ref {
    _instance: any

    get objectNumber() {
        return this._instance.objectNumber
    }

    set objectNumber(value: number) {
        this._instance.objectNumber = value
    }

    get generation() {
        return this._instance.generation
    }

    set(value: number) {
        this._instance.generation = value
    }

    constructor(instance: any) {
        this._instance = instance
    }

    write(output: string): void {
        if (output === null) throw Error('output must be a valid output file path')
        this._instance.write(output)
    }

    isIndirect(): boolean {
        return this._instance.isIndirect()
    }

    toString(): string {
        return this._instance.toString()
    }

    deref(): Obj {
        return new Obj(this._instance.getObject())
    }

}