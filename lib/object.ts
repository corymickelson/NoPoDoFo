import { Ref } from "./reference";
import { Dictionary } from "./dictionary";
import { Arr } from "./arr";

export type PDType =
    'Boolean'
    | 'Number'
    | 'Name'
    | 'Real'
    | 'String'
    | 'Array'
    | 'Dictionary'
    | 'Reference'
    | 'RawData'

/**
 * @desc This class represents a PDF indirect Object in memory
 *      It is possible to manipulate the stream which can be appended to the object(if the object is of underlying type dictionary)
 *      A PdfObject is uniquely identified by an object number and generation number
 *      The object can easily be written to a file using the write function
 *
 * @todo New instance object not yet supported. Objects can only be instantiated from an existing object
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

    setInstance(instance: any): void {
        this._instance = instance
    }

    hasStream(): boolean {
        return this._instance.hasStream()
    }

    /**
     * @desc Calculates the byte offset of key from the start of the object if the object was written to disk at the moment of calling the function
     *      This function is very calculation intensive!
     *
     * @param {string} key object dictionary key
     * @returns {number} - byte offset
     */
    getOffset(key: string): Promise<number> {
        return this._instance.getOffset(key)
    }

    // write(output: string): Promise<string> {
    //     return this._instance.write(output)
    // }
    write(output: string, cb: Function): void {
        this._instance.write(output, cb)
    }

    /**
     * @desc This function compresses any currently set stream using the FlateDecode algorithm.
     *  JPEG compressed streams will not be compressed again using this function.
     *  Entries to the filter dictionary will be added if necessary.
     */
    flateCompressStream(): void {
        this._instance.flateCompressStream()
    }

    /**
     * @desc Dynamically load this object and any associated stream from a PDF file
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

    // asArray(): Arr {
    //     const i = this._instance.getArray()
    //     return new Arr(i)
    // }

    asArray(): Array<Obj> {
        const i = this._instance.getArray(),
            array = i.toArray(),
            length = array.length,
            mutableCheck = () => {
                if (!i.mutable) {
                    throw Error('Array is not mutable. To make changes to the underlying PoDoFo::PdfArray you must first set the mutable property to true.')
                }
            }
        let dirty: boolean = i.dirty
        //array:Array<Obj> = i.getArray().map((j: any) => new Obj(j)),
        const ap = new Proxy(i, {
            get(target, prop) {
                let int = parseInt((prop as string))
                if (!Number.isNaN(int)) prop = int
                if (typeof prop === 'string') {
                    switch (prop) {
                        case 'pop':
                            mutableCheck()
                            return () => {
                                const i = length - 1,
                                    item = (dirty ? (i as any).toArray() : array || (i as any).toArray())[i]
                                        (i as any).remove(i)
                                dirty = true
                                return item
                            }
                        // return () => target.remove(target.length)
                        case 'unshift':
                            mutableCheck()
                            return () => {
                                const item = (dirty ? (i as any).toArray() : array || (i as any).toArray())[0]
                                (i as any).remove(0)
                                dirty = true
                                return item
                            }
                        case 'push':
                            mutableCheck()
                            return (v: Obj) => {
                                dirty = true
                                i.add(v, length)
                            }
                        case 'shift':
                            mutableCheck()
                            return (v: Obj) => {
                                dirty = true
                                i.add(v, 0)
                            }
                        case 'length':
                            return (dirty ? (i as any).toArray() : array || (i as any).toArray()).length
                        default: // catch all, and apply to native js array
                            // const array = target.dirty ? target.asArray() : ac || target.asArray()
                            return (...args: any[]) => {
                                dirty = true
                                return dirty ? (i as any).toArray() : array || (i as any).toArray()[prop].call(args)
                            }
                    }
                } else if (typeof prop === 'number' && prop > -1 && prop < target.length) {
                    const arr = i.dirty ? (i as any).toArray() : array || (i as any).toArray()
                    return new Obj(arr[prop])
                } else {

                }
            },
            set(target, prop, value): boolean {
                if (value instanceof Obj && typeof prop === 'number') {
                    target.add(value, prop)
                    return true
                }
                return false
            }
        })

        return ap
    }

    asDictionary(): Dictionary {
        return new Dictionary(this)
    }

    asReference(): Ref {
        const i = this._instance.getReference()
        return new Ref(i)
    }

    asBuffer(): Buffer {
        return this._instance.getRawData()
    }
}

