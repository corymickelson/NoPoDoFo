import {IRef, Ref} from "./reference";
import {IVecObjects} from "./vecObjects";
import {Dictionary, IDictionary} from "./dictionary";
import {Arr, IArr} from "./arr";

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

export interface IObj {
    stream: any
    length: number
    owner: IVecObjects
    reference: IRef
    _instance: any
    type: PDType

    hasStream(): boolean

    getOffsetSync(key: string): number

    getOffset(key: string, cb: (e: Error, v: number) => void): void

    write(output: string, cb: (e: Error, output: string) => void): void

    writeSync(output: string): void

    flateCompressStream(): void

    delayedStreamLoad(): void

    asBool(): boolean

    asString(): string

    asName(): string

    asReal(): number

    asNumber(): number

    asArray(): IArr

    asDictionary(): Dictionary

    asReference(): IRef

    asBuffer(): Buffer
}

/**
 * @desc This class represents a PDF indirect Object in memory
 *      It is possible to manipulate the stream which can be appended to the object(if the object is of underlying type dictionary)
 *      A PdfObject is uniquely identified by an object number and generation number
 *      The object can easily be written to a file using the write function
 *
 * @todo New instance object not yet supported. Objects can only be instantiated from an existing object
 */
export class Obj implements IObj {
    _instance: any

    get reference() {
        return this._instance.reference
    }

    set reference(value: any) {
        throw Error("Reference may not be set manually")
    }

    get owner() {
        return this._instance.owner
    }

    set owner(value: IVecObjects) {
        this._instance.owner = value
    }

    get length() {
        return this._instance.length
    }

    set length(value: any) {
        throw Error("Object Length may not be set manually")
    }

    get stream() {
        return this._instance.stream
    }

    set stream(value: any) {
        throw Error("Object stream may not be set manually")
    }

    get type() {
        return this._instance.type
    }

    set type(value: PDType) {
        throw Error('Can not set type')
    }

    constructor(instance: any) {
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
    getOffsetSync(key: string): number {
        return this._instance.getOffsetSync(key)
    }

    getOffset(key: string, cb: Function): void {
        this._instance.getOffset(key, cb)
    }

    /**
     * @desc Write the complete object to a file
     */
    writeSync(output: string): void {
        try {
            this._instance.writeSync(output)
        } catch (error) {
            throw error
        }
    }

    write(output: string, cb: (e: Error, output: string) => void): void {
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

    asArray(): IArr {
        const i = this._instance.getArray()
        return new Arr(i)
    }

    asDictionary(): Dictionary {
        return new Dictionary(this)
    }

    asReference(): IRef {
        const i = this._instance.getReference()
        return new Ref(i)
    }

    asBuffer(): Buffer {
        throw Error("unimplmented")
    }
}

