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
        let mutableCheck =
                () => {
                    if (internal.immutable) {
                        throw Error(
                            'Array is not mutable. To make changes to the underlying PoDoFo::PdfArray you must first set the mutable property to true.')
                    }
                },
            propIndex = (v: any): number | null => {
                const index = parseInt((v as string));
                if (!Number.isNaN(index)) {
                    return null
                }
                return index
            }

        return new Proxy(internal, {
            get(target: NPDFInternal, prop: any) {
                const int = propIndex(prop)
                if (int) prop = int
                if (typeof prop === 'string') {
                    switch (prop) {
                        case 'pop':
                            mutableCheck()
                            return () => {
                                const item = target.at(target.length - 1)
                                target.remove(target.length - 1)
                                return new Obj(item)
                            }
                        case 'unshift':
                            mutableCheck()
                            return (...v: Array<Obj>) => {
                                if (v.every(i => i instanceof Obj)) {
                                    v.forEach((item, index, array) => {
                                        try {
                                            target.add(item._instance, 0)
                                        } catch (e) {
                                            console.error(e)
                                            throw e
                                        }
                                    })
                                    return target.length
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
                                            target.add(item._instance, target.length - 1)
                                        } catch (e) {
                                            console.error(e)
                                            throw e
                                        }
                                    })
                                    return target.length
                                } else {
                                    throw TypeError()
                                }
                            }
                        case 'shift':
                            mutableCheck()
                            return () => {
                                const item = target.at(0)
                                target.remove(0)
                                return new Obj(item)
                            }
                        case 'length':
                            return target.length
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
                else if (typeof prop === 'number' && prop > -1 && prop < target.length) {
                    if (prop < 0 || prop > target.length) {
                        throw new RangeError()
                    }
                    return new Obj(target.at(prop))
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
                    target.add(value, prop)
                    return true
                }
                return false
            },
            deleteProperty(target: NPDFInternal, prop: any): boolean {
                mutableCheck()
                const index = propIndex(prop)
                if (!prop) {
                    console.warn(`When prop is null the entire array is cleared`)
                    target.clear()
                }
                target.remove(index)
                return true
            }
        })
    }

    asObject(): Object {
        const internal: NPDFInternal = new __mod.Dictionary(this._instance)
        return new Proxy<Object>(internal, {
            get(target: NPDFInternal, prop: any) {
                if (target.immutable) {
                    throw EvalError('Object is immutable')
                } else if (target.hasKey(prop)) {
                    return new Obj(target.getKey(prop))
                } else {
                    throw EvalError(`Property ${prop} does not exist on object.`)
                }
            },
            set(target: NPDFInternal, prop: any, value: Obj): boolean {
                if (target.immutable) {
                    throw EvalError()
                } else {
                    target.addKey(prop, value.hasOwnProperty('_instance') ? value._instance : value)
                }
                return true
            },
            deleteProperty(target: NPDFInternal, prop: any): boolean {
                if (target.immutable) {
                    throw EvalError()
                } else if(target.hasKey(prop)){
                    console.info(`Clearing property ${prop}`)
                    target.removeKey(prop)
                } else if(!prop) {
                    target.clear()
                    console.warn('When prop is null the entire object is cleared')
                }
                return true
            },
            has(target: NPDFInternal, prop: any): boolean {
                return target.hasKey(prop)
            },
            defineProperty(target: NPDFInternal, prop: any, descriptor: any): boolean {
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
            getOwnPropertyDescriptor(target: NPDFInternal, prop: any): PropertyDescriptor {
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
            ownKeys(target: NPDFInternal): Array<string> {
                return ['stream', 'type', 'length', 'reference', 'immutable', 'hasStream', 'getOffset', 'write',
                    'flateCompressStream', 'delayedStreamLoad', 'getBool', 'getNumber', 'getRea', 'getString',
                    'getName', 'getArray', 'getReference', 'getRawData']
            },
            isExtensible(target: NPDFInternal): boolean {
                return !target.immutable
            },
            preventExtensions(target: NPDFInternal): boolean {
                return !target.immutable
            },
            setPrototypeOf(target: NPDFInternal, value: any): boolean {
                console.warn('The prototype of this object can not be set')
                return false
            }
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
