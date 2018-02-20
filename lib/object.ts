/**
 * This file is part of the NoPoDoFo (R) project.
 * Copyright (c) 2017-2018
 * Authors: Cory Mickelson, et al.
 * 
 * NoPoDoFo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NoPoDoFo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import { __mod } from "./document";
import { Ref } from "./reference";
import { Stream } from "stream";

export type NPDFInternal = any

export type CoerceKeyType = 'boolean' | 'long' | 'name' | 'real'

export type PDType = 'Boolean' | 'Number' | 'Name' | 'Real' | 'String' | 'Array' |
    'Dictionary' | 'Reference' | 'RawData'

export interface NArray {
    [key: number]: Obj
    immutable: boolean
    length: number
    unshift: (...v:Array<Obj>) => number
    push: (...v:Array<Obj>) => number
    pop: () => Obj
    shift: () => Obj
}
export interface NObj {
    reference: number
    length: number
    stream: Stream
    type: PDType
    hasStream():boolean
    getOffset(key: string): Promise<number>
    write(output: string, cb: Function): void
    flateCompressStream(): void
    delayedStreamLoad(): void
    asBool(): boolean
    asString(): string
    asName(): string
    asReal(): number
    asNumber(): number
    asArray(): NArray
    asObject(): { [key: string]: Obj }
    asReference(): Ref
    asBuffer(): Buffer
}
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

export class Obj implements NObj {
    private _instance: any

    get reference() {
        return this._instance.reference
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
     * a console message will be provided.  
     * @returns {Array<Obj>}
     */
    asArray(): NArray {
        const internal: NPDFInternal = this._instance.getArray()
        let data: Array<Obj> = internal.toArray()
        const mutableCheck =
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
                if (typeof (int) === 'number') prop = int
                if (typeof prop === 'string') {
                    switch (prop) {
                        case 'immutable':
                            return internal.immutable
                        case 'pop':
                            mutableCheck()
                            return () => {
                                const item = data.pop()
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
                                    if (data.length !== internal.length) {
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
                                const item = data.shift()
                                internal.remove(0)
                                return new Obj(item)
                            }
                        case 'length':
                            if (data.length !== internal.length) {
                                throw Error("Internal array out of sync")
                            }
                            return data.length
                        // case 'concat':
                        // case 'copyWithin':
                        //     throw TypeError(`NoPoDoFo does not yet support the creation of new PdfArray types.`)
                        // case 'entries':
                        //     throw TypeError(`NoPoDoFo does not yet support this method on PdfArray types.`)
                        default:
                            throw TypeError(`${prop} is not supported`)
                    }
                }
                else if (typeof prop === 'number' && prop > -1 && prop < data.length) {
                    if (prop < 0 || prop > data.length) {
                        throw new RangeError()
                    }
                    return new Obj(data[prop])
                } else {
                    throw EvalError(`NoPoDoFo array does not support ${prop}`)
                }
            },
            set(target, prop, value): boolean {
                if (prop === 'immutable' && typeof (value) === 'boolean') {
                    internal.immutable = value
                    return true
                } else {
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
                }

            },
            defineProperty(target: NPDFInternal, prop: any, descriptor: any): boolean {
                if (prop === 'immutable') {
                    if (typeof (descriptor) === 'boolean') {
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

    asObject(): { [key: string]: Obj } {
        const internal: NPDFInternal = this._instance.getDictionary()
        let data = internal.toObject()
        return new Proxy<{ [key: string]: Obj }>(internal, {
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
                    throw TypeError(`This dictionary object does not contain property: ${prop}`)
                }
                const immutable = internal.immutable
                return {
                    writable: !immutable,
                    value: data[prop],
                    configurable: !immutable,
                    enumerable: true
                }
            },
            ownKeys(target: NPDFInternal): Array<string> {
                return Object.keys(data)
            },
            isExtensible(target: NPDFInternal): boolean {
                return false
            },
            preventExtensions(target: NPDFInternal): boolean {
                return false
            },
            setPrototypeOf(target: NPDFInternal, value: any): boolean {
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
