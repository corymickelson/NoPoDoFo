import {__mod} from "./document";
import {Ref} from "./reference";

export type NPDFInternal = any

export type CoerceKeyType = 'boolean' | 'long' | 'name' | 'real'

export type PDType = 'Boolean' | 'Number' | 'Name' | 'Real' | 'String' | 'Array' |
    'Dictionary' | 'Reference' | 'RawData'

/**
 * @desc This class represents a PDF indirect Object in memory
 *      It is possible to manipulate the stream which can be appended to the
 * object(if the object is of underlying type dictionary) A PdfObject is
 * uniquely identified by an object number and generation number The object can
 * easily be written to a file using the write function
 *
 * @todo New instance object not yet supported. Objects can only be instantiated
 * from an existing object
 */
export class Obj {
    private _instance: any

    get reference() {
        return this._instance.reference
    }

    get owner() {
        return this._instance.owner
    }

    set owner(value: Array<Obj>) {
        this._instance.owner = value
    }

    get length() {
        return this._instance.length
    }

    get stream() {
        return this._instance.stream
    }

    get type() {
        return this._instance.type
    }

    constructor(instance: any) {
        this._instance = instance
    }

    hasStream(): boolean {
        return this._instance.hasStream()
    }

    /**
     * @desc Calculates the byte offset of key from the start of the object if the
     * object was written to disk at the moment of calling the function This
     * function is very calculation intensive!
     *
     * @param {string} key object dictionary key
     * @returns {number} - byte offset
     */
    getOffset(key: string): Promise<number> {
        return this._instance.getOffset(key)
    }

    write(output: string, cb: Function): void {
        this._instance.write(output, cb)
    }

    /**
     * @desc This function compresses any currently set stream using the
     * FlateDecode algorithm. JPEG compressed streams will not be compressed again
     * using this function. Entries to the filter dictionary will be added if
     * necessary.
     */
    flateCompressStream(): void {
        this._instance.flateCompressStream()
    }

    /**
     * @desc Dynamically load this object and any associated stream from a PDF
     * file
     */
    delayedStreamLoad(): void {
        this._instance.delayedStreamLoad()
    }

    asBool(): boolean {
        return this._instance.getBool()
    }

    asString(): string {
        return this._instance.getString()
    }

    asName(): string {
        return this._instance.getName()
    }

    asReal(): number {
        return this._instance.getReal()
    }

    asNumber(): number {
        return this._instance.getNumber()
    }

    /**
     * The asArray method returns an Array Proxy (or an Object that is wrapped in a proxy that exposes methods similar to
     * an array, which modifies the underlying PdfArray Object). This is NOT an array, but provides a data structure
     * as close to an array as possible to help user's view/modify the data. If a method or property is not supported
     * a console message will be provided. For more information on NoPoDoFo Array see the guides.
     * @returns {Array<Obj>}
     */
    asArray(): Array<Obj> {
        const internal: NPDFInternal = this._instance.getArray()
        let data: Array<Obj> = internal.toArray(),
            mutableCheck =
                () => {
                    if (internal.immutable) {
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

        return new Proxy(internal, {
            get(target: NPDFInternal, prop: any) {
                const int = propIndex(prop)
                if (typeof(int) === 'number') prop = int
                if (typeof prop === 'string') {
                    switch (prop) {
                        case 'pop':
                            mutableCheck()
                            return () => {
                                const item = data.pop()//internal.at(internal.length - 1)
                                internal.remove(internal.length - 1)
                                return new Obj(item)
                            }
                        case 'unshift':
                            mutableCheck()
                            return (...v: Array<Obj>) => {
                                if (v.every(i => i instanceof Obj)) {
                                    v.forEach((item, index, array) => {
                                        try {
                                            internal.add(item._instance, 0)
                                        } catch (e) {
                                            console.error(e)
                                            throw e
                                        }
                                    })
                                    data.unshift(...v)
                                    if (data.length != internal.length) {
                                        throw Error('Internal array out of sync')
                                    }
                                    return data.length
                                } else {
                                    throw TypeError()
                                }
                            }
                        case 'push':
                            mutableCheck()
                            return (...v: Array<Obj>) => {
                                if (v.every(i => i instanceof Obj)) {
                                    v.forEach((item, index, array) => {
                                        try {
                                            internal.add(item._instance, internal.length - 1)
                                        } catch (e) {
                                            console.error(e)
                                            throw e
                                        }
                                    })
                                    data.push(...v)
                                    if (data.length !== internal.length) {
                                        throw Error("Internal array out of sync")
                                    }
                                    return data.length
                                } else {
                                    throw TypeError()
                                }
                            }
                        case 'shift':
                            mutableCheck()
                            return () => {
                                const item = data.shift()//internal.at(0)
                                internal.remove(0)
                                return new Obj(item)
                            }
                        case 'length':
                            if (data.length !== internal.length) {
                                throw Error("Internal array out of sync")
                            }
                            return data.length
                        case 'concat':
                        case 'copyWithin':
                            console.warn(`NoPoDoFo does not yet support the creation of new PdfArray types.`)
                            break
                        case 'entries':
                            console.warn(`NoPoDoFo does not yet support this method on PdfArray types.`)
                            break
                        default:
                            console.warn(`${prop} is not supported`)
                            return false
                    }
                }
                else if (typeof prop === 'number' && prop > -1 && prop < data.length) {
                    if (prop < 0 || prop > data.length) {
                        throw new RangeError()
                    }
                    return new Obj(data[prop])
                } else {
                    throw EvalError()
                }
            },
            set(target, prop, value): boolean {
                mutableCheck()
                const index = propIndex(prop);
                if (!index) {
                    throw RangeError()
                }
                if (value instanceof Obj && typeof prop === 'number') {
                    internal.add(value, prop)
                    data[prop] = value
                    return true
                }
                return false
            },
            defineProperty(target: NPDFInternal, prop: any, descriptor: any): boolean {
                if (prop === 'immutable') {
                    if (typeof(descriptor) === 'boolean') {
                        internal.immutable = descriptor
                    } else {
                        return false
                    }
                }
                return true
            },
            deleteProperty(target: NPDFInternal, prop: any): boolean {
                mutableCheck()
                const index = propIndex(prop)
                if (!index) {
                    throw TypeError()
                }
                if (!prop) {
                    console.warn(`When prop is null the entire array is cleared`)
                    internal.clear()
                    data = []
                } else {
                    internal.remove(index)
                    data.splice(index, 1)
                }
                return true
            }
        })
    }

    asObject(): {[key:string]: Obj} {
        // const internal: NPDFInternal = new __mod.Dictionary(this._instance)
        const internal: NPDFInternal = this._instance.getDictionary()
        let data = internal.toObject()
        return new Proxy<{[key:string]: Obj}>(internal, {
            get(target: NPDFInternal, prop: any) {
                if (internal.immutable) {
                    throw EvalError('Object is immutable')
                }
                else if (data.hasOwnProperty(prop)) {
                    return new Obj(data[prop])
                }
                else if (internal.hasKey(prop)) {
                    return new Obj(internal.getKey(prop))
                } else {
                    return null
                }
            },
            set(target: NPDFInternal, prop: any, value: Obj): boolean {
                if (internal.immutable) {
                    throw EvalError()
                } else {
                    internal.addKey(prop, value.hasOwnProperty('_instance') ? value._instance : value)
                    data = internal.toObject()
                }
                return true
            },
            deleteProperty(target: NPDFInternal, prop: any): boolean {
                if (internal.immutable) {
                    throw EvalError()
                } else if (internal.hasKey(prop)) {
                    console.info(`Clearing property ${prop}`)
                    internal.removeKey(prop)
                } else if (!prop) {
                    internal.clear()
                    console.warn('When prop is null the entire object is cleared')
                }
                data = internal.toObject()
                return true
            },
            has(target: NPDFInternal, prop: any): boolean {
                return data.hasOwnProperty(prop)
            },
            defineProperty(target: NPDFInternal, prop: any, descriptor: any): boolean {
                if (internal.immutable) {
                    throw EvalError()
                }
                const properties = ['enumerable', 'configurable', 'writable', 'value', 'get', 'set']
                if (properties.indexOf(prop) === -1) {
                    throw EvalError(`${prop} is not a definable property`)
                } else if (prop === 'writable' || prop === 'value') {
                    console.info('NoPoDoFo primitive array uses a JS Proxy and exposes definable properties: writable, value')
                    if (prop === 'writable') {
                        console.warn('Setting the immutable property will define the writable value for the entire object.')
                        if (typeof descriptor === 'boolean') internal.immutable = descriptor
                        else throw TypeError('The immutable flag requires a boolean value')
                    }
                } else {
                    return false
                }
                return true
            },
            getOwnPropertyDescriptor(target: NPDFInternal, prop: any): PropertyDescriptor {
                if (!data.hasOwnProperty(prop) || !internal.hasKey(prop)) {
                    return {}
                }
                const immutable = internal.immutable
                return {
                    writable: !immutable,
                    value: data[prop], //internal.getKey(prop),
                    configurable: !immutable,
                    enumerable: true
                }
            },
            ownKeys(target: NPDFInternal): Array<string> {
                // return ['stream', 'type', 'length', 'reference', 'immutable', 'hasStream', 'getOffset', 'write',
                //     'flateCompressStream', 'delayedStreamLoad', 'getBool', 'getNumber', 'getRea', 'getString',
                //     'getName', 'getArray', 'getReference', 'getRawData']
                return Object.keys(data)
            },
            // isExtensible(target: NPDFInternal): boolean {
            //     return !internal.immutable
            // },
            // preventExtensions(target: NPDFInternal): boolean {
            //     return !internal.immutable
            // },
            // setPrototypeOf(target: NPDFInternal, value: any): boolean {
            //     console.warn('The prototype of this object can not be set')
            //     return false
            // }
        })
    }

    asReference(): Ref {
        const i = this._instance.getReference()
        return new Ref(i)
    }

    asBuffer(): Buffer {
        return this._instance.getRawData()
    }
}
