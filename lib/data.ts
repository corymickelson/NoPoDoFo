import { __mod } from "./document";

export type External = any
export class Data {
    private _instance: any
    get value(): string {
        return this._instance.value
    }
    constructor(value: string | External) {
        if (typeof value === 'string') {
            this._instance = new __mod.Data(value)
        } else {
            this._instance = value
        }
    }
    write(output: string): void {
        if (!output) {
            throw Error("outpust must be a valid path")
        }
        this._instance.write(output)
    }
}