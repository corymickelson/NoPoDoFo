import { IDictionary } from "./dictionary";
import { IReference } from "./reference";
import { IVecObjects } from "./vecObjects";

export interface IPDObject {
    stream: any
    length: number
    type: string
    owner: IVecObjects
    reference: IReference

    hasStream(): boolean
    getOffset(key: string): number
    write(output:string): void
    flateCompressStream(): void
    delayedStreamLoad(): void
}

/**
 * @desc This class represents a PDF indirect Object in memory
 *      It is possible to manipulate the stream which can be appended to the object(if the object is of underlying type dictionary)
 *      A PdfObject is uniquely identified by an object number and generation number
 *      The object can easily be written to a file using the write function
 * 
 * @todo New instance object not yet supported. Objects can only be instantiated from an existing object
 */
export class PDObject implements IPDObject {
    stream: any;
    length: number;
    type: string;
    owner: IVecObjects;
    reference: IReference;

    constructor(private _instance: any) { }

    hasStream(): boolean {
        return this._instance.hasStream()
    }
    /**
     * @desc Calculates the byte offset of key from the start of the object if the object was written to disk at the moment of calling the function
     *      This function is very calculation instensive!
     * 
     * @param {string} key object dictionary key
     * @returns {number} - byte offset
     */
    getOffset(key: string): number {
        return this._instance.getOffset(key)
    }
    /**
     * @desc Write the complete object to a file
     */
    write(output:string): void {
        try {
            this._instance.write(output)
        } catch (error) {
            throw error
        }
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
}