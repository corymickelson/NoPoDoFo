import {IFieldInfo} from './field'

const mod = require('../src/npdf')

export interface IDocument {

    load(file: string): void

    getPageCount(): number

    getPage(pageN: number): IPage

    mergeDocument(doc: string): void

    deletePage(pageIndex: number): void

    getVersion(): number

    isLinearized(): boolean

    write(file: string): void
}
export interface IPage {
    getRotation(): number

    getNumFields(): number

    /**
     * @description Set page rotate dict entry. Value must be an increment of 90, where 0 is vertical.
     */
    setRotation(degree: number): void

    /**
     * @description Get information on all fields on the page
     */
    getFields(): Array<IFieldInfo>

    /**
     * @description Get field index of fieldname.
     */
    getFieldIndex(fieldName: string): number
}

/**
 * @class Document
 * @example
 * import {Document} from 'npdf'
 * const doc = new Document('/path/to/file')
 * const pageCount = doc.getPageCount()
 * const page = doc.getPage(0) // pages are zero indexed
 * const mergeDocs = doc.mergeDocument('/path/to/other/doc')
 * doc.setPassword('secret')
 * doc.write('/file/destination')
 * const fileBuffer = fs.readFile('/file/destination', (e,d) => e ? e : d)
 * // do something with the file or the file buffer
 */
export class Document implements IDocument {

    private _instance: any
    private _loaded: boolean = false;
    private _pageCount:number
    private _password:string

    get pageCount() {
        return this._pageCount
    }

    set password(value:string) {
        this._password = value;
    }

    get password() {
        throw Error("Can not get password from loaded document")
    }

    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @returns void
     */
    constructor(file: string) {
        this._instance = new mod.Document()
        if(file) {
            this._instance.load(file)
            this._pageCount = this._instance.getPageCount()            
            this._loaded = true
        }
    }

    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     */
    load(file: string): void {
        this._instance.load(file)
        this._loaded = true
    }

    getPageCount(): number {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getPageCount()
    }

    getPage(pageN: number): IPage {
        if(this.pageCount > pageN || this.pageCount < 0) {
            throw new RangeError("pageN out of range")
        }
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        const page: IPage = this._instance.getPage(pageN)
        return page;
    }

    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     */
    mergeDocument(doc: string): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        this._instance.mergeDocument(doc)
    }

    deletePage(pageIndex: number): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        if(this.pageCount < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range')
        }
        this._instance.deletePage(pageIndex)
    }

    getVersion(): number {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getVersion()
    }

    isLinearized(): boolean {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.isLinearized()
    }

    write(file: string): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        this._instance.write(file)
    }

}