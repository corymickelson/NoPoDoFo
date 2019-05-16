import {nopodofo} from "../index";
import {NDocument} from "./NDocument";
import {NObject} from "./NObject";
import Ref = nopodofo.Ref;

export class NArray {
    private self: nopodofo.Array

    get immutable(): boolean {
        return this.self.immutable
    }

    set immutable(value: boolean) {
        this.self.immutable = value;
    }

    get dirty(): boolean {
        return this.self.dirty;
    }

    set dirty(value: boolean) {
        this.self.dirty = value;
    }

    get length(): number {
        return this.self.length
    }

    constructor(private parent: NDocument, private owner: NObject) {
        if (this.owner.type !== 'Array') {
            throw TypeError('NObject not of type Array')
        }
        this.self = this.owner.getArray()
    }

    private resolveRef(f: Function): NObject {
        let o = f()
        if (o instanceof Ref) {
            o = this.parent.getObject(o)
        }
        return new NObject(this.parent, o)
    }

    at(i: number): NObject {
        return this.resolveRef(() => this.self.at(i))
    }

    clear(): void {
        this.self.clear()
    }

    pop(): NObject {
        return this.resolveRef(() => this.self.pop())
    }

    push(v: NObject): void {
        this.self.push(v.self)
    }

    write(destination: string): void {
        this.self.write(destination)
    }

    asArray(): Array<string | number | Object | Array<any> | null> {
        return this.self.asArray()
    }

}
