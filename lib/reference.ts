import {Obj} from "./object";


export class Ref {

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

    constructor(private _instance: any) {
    }

    write(output: string): void {
        if (output === null) throw Error('output must be a valid output file path')
        this._instance.write(output)
    }

    /**
     * Check if ref points to an indirect object. Object is indirect if both object and generation numbers are not 0
     */
    isIndirect(): boolean {
        return this._instance.isIndirect()
    }

    toString(): string {
        return this._instance.toString()
    }

    /**
     * If ref is an indirect object, de-reference ref to PdfObject
     */
    deref(): Obj {
        return new Obj(this._instance.getObject())
    }

}