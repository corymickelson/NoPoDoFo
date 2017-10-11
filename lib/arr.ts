import { IObj, Obj } from "./object";

export interface IArr {
    dirty: boolean
    length: number
    toArray(): Array<IObj>
    contains(key: string): boolean
    indexOf(key: string): number
    write(output: string): void
    at(index:number): IObj
}

/**
 * @class Arr wraps PdfArray
 * The underlying pdf array is using vector<PdfObject>
 * The methods declared on Arr are methods PoDoFo has optimized for
 * Pdf array's.
 * If you would like to use Arr as an instance of a javascript array call ToArray()
 */
export class Arr implements IArr {
    dirty: boolean;

    get length() {
        return this._instance.length
    }

    set length(value: number) {
        throw Error("Can not set length")
    }

    constructor(public _instance: any) { }

    /**
     * Converts PdfArray to javascript array.
     * Any modifications to javascript array will NOT be persisted to PdfArray.
     * Use for lookup only
     * @returns {Array<IObj>}
     */
    toArray(): Array<IObj> {
        const init:Array<any> = this._instance.toArray()
        return init.map(i => new Obj(i))
    }

    /**
     * Search PdfArray for key
     * @param {string} key
     * @returns {boolean}
     */
    contains(key: string): boolean {
        return this._instance.contains(key)
    }

    /**
     * Find index of key
     * @param {string} key
     * @returns {number}
     */
    indexOf(key: string): number {
        return this._instance.indexOf(key)
    }

    /**
     * Writes the array to a file. Useful for debugging
     * @param {string} output
     */
    write(output: string): void {
        this._instance.write(output)
    }

    /**
     * Retrieve the PdfObject at index
     * @param {number} index
     * @returns {IObj}
     */
    at(index:number): IObj {
        const item = this._instance.getIndex(index)
        return new Obj(item)
    }
}
