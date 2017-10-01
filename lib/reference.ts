export interface IRef {
    object: number
    generation: number

    write(output: string): void

    toString(): string

    isIndirect(): boolean
}

export class Ref implements IRef {
    _instance: any

    get object() {
        return this._instance.object
    }

    set object(value: number) {
        this._instance.object = value
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

}