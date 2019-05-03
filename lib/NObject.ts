import {Callback, nopodofo} from "../index";
import {NDocument} from "./NDocument";

export class NObject implements nopodofo.Object {
    get immutable(): boolean {
        return this.obj.immutable;
    }

    set immutable(value: boolean) {
        this.obj.immutable = value;
    }

    get type(): "Boolean" | "Number" | "Name" | "Real" | "String" | "Array" | "Dictionary" | "Reference" | "RawData" {
        return this.obj.type;
    }

    get stream(): nopodofo.Stream {
        return this.obj.stream;
    }

    get length(): number {
        return this.obj.length;
    }

    get reference(): nopodofo.Ref {
        return this.obj.reference;
    }

    hasStream(): boolean {
        return this.obj.hasStream()
    }

    getOffset(key: string): Promise<number> {
        return new Promise((resolve, reject) => this.obj.getOffset(key, (err, value) => err ? reject(err) : resolve(value)))
    }

    write(output: string, cb: Callback<string>): void | Promise<string> {
        if (cb) {
            this.obj.write(output, cb)
        } else {
            return new Promise((resolve, reject) => (err: Error, value: string) => err ? reject(err) : resolve(value))
        }
    }

    flateCompressStream(): void {
        this.obj.flateCompressStream()
    }

    delayedStreamLoad(): void {
        this.obj.delayedStreamLoad()
    }

    getBool(): boolean {
        return this.obj.getBool()
    }

    getDictionary(): nopodofo.Dictionary {
        return this.obj.getDictionary()
    }

    getString(): string {
        return this.obj.getString()
    }

    getName(): string {
        return this.obj.getName()
    }

    getReal(): number {
        return this.obj.getReal()
    }

    getNumber(): number {
        return this.obj.getNumber()
    }

    getArray(): nopodofo.Array {
        return this.obj.getArray()
    }

    getRawData(): Buffer {
        return this.obj.getRawData()
    }

    clear(): void {
        this.obj.clear()
    }

    resolveIndirectKey(key: string): nopodofo.Object {
        return this.obj.resolveIndirectKey(key)
    }

    constructor(private parent: NDocument, private obj: nopodofo.Object) {
    }
}
