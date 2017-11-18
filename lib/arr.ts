import { Obj } from "./object"

/**
 * @class Arr wraps PdfArray
 * The underlying pdf array is using vector<PdfObject>
 * The methods declared on Arr are methods PoDoFo has optimized for
 * Pdf array's.
 * If you would like to use Arr as an instance of a javascript array call ToArray()
 */
export class Arr {
    dirty: boolean;
    private _array: Array<Obj>;

    get length() {
        return this._instance.length
    }

    static initialize(instance: any) {
        const nArr = new Arr(instance)
        nArr._array = nArr.toArray()
        const nProxy = new Proxy(nArr, {
            get(target, prop) {
                return target._array[prop as number]
            },
            set(target, prop, value):boolean {
                let success = true
                if(value instanceof Obj === false) {
                    success = false
                    throw Error('pdf array can only contain an instance of Obj')
                }
                target._array[prop as number] = value
                nArr.push(value)
                return success
            }
        })
        return nArr
    }

    constructor(private _instance: any) { }

    /**
     * Converts PdfArray to javascript array.
     * Any modifications to javascript array will NOT be persisted to PdfArray.
     * Use for lookup only
     * @returns {Array<Obj>}
     */
    toArray(): Array<Obj> {
        const init: Array<any> = this._instance.toArray()
        this._array = init.map(i => new Obj(i))
        return this._array
    }

    push(value: Obj): void {
        this._instance.push(value)
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
     * @returns {Obj}
     */
    at(index: number): Obj {
        const item = this._instance.getIndex(index)
        return new Obj(item)
    }
}
