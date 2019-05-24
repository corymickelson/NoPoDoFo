import {Callback, nopodofo} from "../index";
import {NDocument} from "./NDocument";
import Ref = nopodofo.Ref;

export class NObject {
    private ap(v: nopodofo.Array): Array<any> {
        let mutableCheck =
                () => {
                    if (v.immutable) {
                        throw Error(
                            'Array is not mutable. To make changes to the underlying PoDoFo::PdfArray you must first set the mutable property to true.')
                    }
                },
            propIndex = (v: any): number | null => {
                const index = parseInt((v as string));
                if (Number.isNaN(index)) {
                    return null
                }
                return index
            }
        const parent = this.parent
        // @ts-ignore
        return new Proxy<Array<any>>(v, {
            get(target: nopodofo.Array, prop: any) {
                const int = propIndex(prop)
                if (int !== null) prop = int
                if (typeof prop === 'number' && prop > -1 && prop < target.length) {
                    if (prop < 0 || prop > target.length) {
                        throw new RangeError()
                    }
                    let item = target.at(prop)
                    if (item instanceof Ref) {
                        item = (parent.getObject(item) as any) as nopodofo.Object
                    }
                    return new NObject(parent, item)
                } else if (typeof prop === 'string') {
                    switch (prop) {
                        case 'pop':
                            mutableCheck()
                            return () => {
                                const item = target.pop()
                                return new NObject(parent, item)
                            }
                        case 'unshift':
                            mutableCheck()
                            return (...v: Array<NObject>) => {
                                if (v.every(i => i instanceof NObject)) {
                                    v.forEach(vv => {
                                        target.unshift((vv as any).self)
                                    })
                                    return target.length
                                } else {
                                    throw TypeError()
                                }
                            }
                        case 'push':
                            mutableCheck()
                            return (...v: Array<NObject>) => {
                                if (v.every(i => i instanceof NObject)) {
                                    v.forEach(vv => {
                                        target.push((vv as any).self)
                                    })
                                    return target.length
                                } else {
                                    throw TypeError()
                                }
                            }
                        case 'shift':
                            mutableCheck()
                            return () => {
                                const item = target.shift()
                                return new NObject(parent, item)
                            }
                        case 'length':
                            return target.length
                        case 'concat':
                            console.warn('NoPoDoFo Array does not support array concatenation')
                            break
                        case 'copyWithin':
                            console.warn(`NoPoDoFo does not yet support the creation of new PdfArray types.`)
                            break
                        case 'entries':
                            console.warn(`NoPoDoFo does not yet support this method on PdfArray types.`)
                            break
                        case 'dirty':
                            return target.dirty
                        case 'immutable':
                            return target.immutable
                        case 'at':
                            return (i: number) => {
                                let item = target.at(i)
                                if (item instanceof Ref) {
                                    item = parent.getObject(item) as any
                                }
                                return new NObject(parent, item as nopodofo.Object)
                            }
                        case 'clear':
                            target.clear()
                            return
                        case 'write':
                            return (d: string) => {
                                return target.write(d)
                            }
                        case 'asArray':
                            return target.asArray()
                        case 'splice':
                            return (begin: number, endOrItems?: number | NObject[], items?: NObject[]): NObject[] => {
                                if(typeof(endOrItems) === 'number' && !items) {

                                }
                                return []
                        }
                        default:
                            throw Error(`unknown method, could not get ${prop} on NArray data type`)
                    }
                } else {
                    throw EvalError()
                }
            },
            set(target: nopodofo.Array & Array<any>, prop: number | string, value: any): boolean {
                mutableCheck()
                const int = propIndex(prop)
                if (int !== null) prop = int
                if (Number.isNaN(prop as any)) {

                    switch (prop) {
                        case 'dirty':
                            target.dirty = value
                            return true
                        case 'immutable':
                            target.immutable = value
                            return true
                        default:
                            throw Error(`unknown property: ${prop}`)
                    }
                }
                if (value instanceof NObject && typeof prop === 'number') {
                    target.splice(prop, (value as any).self)
                    return true
                }
                return false
            },
            deleteProperty(target: nopodofo.Array, prop: any): boolean {
                mutableCheck()
                const index = propIndex(prop)
                if (!index) {
                    console.warn(`When prop is null the entire array is cleared`)
                    target.clear()
                } else {
                    target.clear(index)
                }
                return true
            }
        })
    }

    private op(v: nopodofo.Dictionary): Object {
        const parent = this.parent
        return new Proxy(v, {
            get(target: nopodofo.Dictionary, prop: any) {
                if (target.immutable) {
                    throw EvalError('Object is immutable')
                } else if (target.hasKey(prop)) {
                    return new NObject(parent, target.getKey(prop))
                } else if (prop == 'dirty' || prop == 'immutable') {
                    return (target as any)[prop]
                } else {
                    throw EvalError(`Property ${prop} does not exist on object.`)
                }
            },
            set(target: nopodofo.Dictionary, prop: any, value: nopodofo.Object): boolean {
                if (target.immutable) {
                    throw EvalError()
                } else {
                    target.addKey(prop, value)
                }
                return true
            },
            deleteProperty(target: nopodofo.Dictionary, prop: any): boolean {
                if (target.immutable) {
                    throw EvalError()
                } else if (target.hasKey(prop)) {
                    console.info(`Clearing property ${prop}`)
                    target.removeKey(prop)
                } else if (!prop) {
                    target.clear()
                    console.warn('When prop is null the entire object is cleared')
                }
                return true
            },
            has(target: nopodofo.Dictionary, prop: any): boolean {
                return target.hasKey(prop)
            },
            defineProperty(target: nopodofo.Dictionary, prop: any, descriptor: any): boolean {
                if (target.immutable) {
                    throw EvalError()
                }
                const properties = ['enumerable', 'configurable', 'writable', 'value', 'get', 'set']
                if (properties.indexOf(prop) === -1) {
                    throw EvalError(`${prop} is not a definable property`)
                } else if (prop === 'writable' || prop === 'value') {
                    console.info('NoPoDoFo primitive array uses a JS Proxy and exposes definable properties: writable, value')
                    if (prop === 'writable') {
                        console.warn('Setting the immutable property will define the writable value for the entire object.')
                        if (typeof descriptor === 'boolean') target.immutable = descriptor
                        else throw TypeError('The immutable flag requires a boolean value')
                    }
                } else {
                    return false
                }
                return true
            },
            getOwnPropertyDescriptor(target: nopodofo.Dictionary, prop: any): PropertyDescriptor {
                if (!target.hasKey(prop)) {
                    throw EvalError(`${prop} does not exist.`)
                }
                return {
                    writable: !target.immutable,
                    value: target.getKey(prop),
                    configurable: !target.immutable,
                    enumerable: true
                }
            },
            ownKeys(target: nopodofo.Dictionary): Array<string> {
                return target.getKeys()
            },
            isExtensible(target: nopodofo.Dictionary): boolean {
                return !target.immutable
            },
            preventExtensions(target: nopodofo.Dictionary): boolean {
                return !target.immutable
            },
            setPrototypeOf(target: nopodofo.Dictionary, value: any): boolean {
                console.warn('The prototype of this object can not be set')
                return false
            }
        })
    }

    get immutable(): boolean {
        return this.self.immutable
    }

    set immutable(value: boolean) {
        this.self.immutable = value
    }

    get type(): "Boolean" | "Number" | "Name" | "Real" | "String" | "Array" | "Dictionary" | "Reference" | "RawData" {
        return this.self.type;
    }

    get stream(): nopodofo.Stream {
        return this.self.stream
    }

    get length(): number {
        return this.self.length
    }

    get reference(): nopodofo.Ref {
        return this.self.reference
    }

    hasStream(): boolean {
        return this.self.hasStream()
    }

    getOffset(key: string): Promise<number> {
        return new Promise((resolve, reject) => this.self.getOffset(key, (err, value) => err ? reject(err) : resolve(value)))
    }

    write(output: string, cb: Callback<string>): void | Promise<string> {
        if (cb) {
            this.self.write(output, cb)
        } else {
            return new Promise((resolve, reject) => (err: Error, value: string) => err ? reject(err) : resolve(value))
        }
    }

    flateCompressStream(): void {
        this.self.flateCompressStream()
    }

    delayedStreamLoad(): void {
        this.self.delayedStreamLoad()
    }

    getBool(): boolean {
        return this.self.getBool()
    }

    getDictionary(): nopodofo.Dictionary {
        return this.self.getDictionary()
    }

    getString(): string {
        return this.self.getString()
    }

    getName(): string {
        return this.self.getName()
    }

    getReal(): number {
        return this.self.getReal()
    }

    getNumber(): number {
        return this.self.getNumber()
    }

    getArray(): Array<any> {
        const item = this.self.getArray()
        return this.ap(item)
    }

    getRawData(): Buffer {
        return this.self.getRawData()
    }

    clear(): void {
        this.self.clear()
    }

    resolveIndirectKey(key: string): nopodofo.Object {
        return this.self.resolveIndirectKey(key)
    }

    constructor(private parent: NDocument, public self: nopodofo.Object) {
    }
}
