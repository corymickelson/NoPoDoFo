import {exists, existsSync} from 'fs'
import { Obj } from './object';
import { Page } from './page';
import { Encrypt, EncryptInitOption } from './encrypt';

export const __mod = require('../build/Release/npdf.node')

export interface IDocument {
    _instance: any
    encrypt: Encrypt

    load(file: string, update?: boolean): void

    getPageCount(): number

    getPage(pageN: number): Page

    getObjects(): Array<Obj>

    mergeDocument(doc: string): void

    deletePage(pageIndex: number): void

    getVersion(): number

    isLinearized(): boolean

    write(file?: string): Promise<string|Buffer>

    createEncrypt(option: EncryptInitOption): Encrypt

    getTrailer():Obj

    freeObjMem(target:Obj): void
}

/**
 * @class Document
 * @desc Document represents a PdfMemDocument, construct from an existing pdf document.
 * Document is the core class for reading and manipulating PDF files and writing them back to disk.
 * Document was designed to allow easy access to the object structur of a PDF file.
 * Document should be used whenever you want to change the object structure of a PDF file.
 */
export class Document implements IDocument {

    _instance: any
    private _loaded: boolean = false;
    private _pageCount: number
    private _password: string

    get pageCount() {
        return this._pageCount
    }

    set password(value: string) {
        this._password = value;
    }

    get password() {
        throw Error("Can not get password from loaded document")
    }

    set encrypt(instance: Encrypt) {
        if (instance.option) this._instance.encrypt = instance.option
        else {
            throw Error("Set document encrypt with an instance of Encrypt with the optional EncryptInitOption defined at construction")
        }
    }

    get encrypt():Encrypt {
        const instance = this._instance.encrypt
        return new Encrypt(instance)
    }
    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @param update
     * @returns void
     */
    constructor(file: string, update: boolean = false) {
        this._instance = new __mod.Document()
        if (file) {
            if(!existsSync(file))
                throw new Error('file not found')
            this._instance.load(file, update)
            this._pageCount = this._instance.getPageCount()
            this._loaded = true
        }
    }

    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     * @param update
     */
    load(file: string, update: boolean = false): void {
        this._instance.load(file, update)
        this._loaded = true
    }

    getPageCount(): number {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getPageCount()
    }

    getPage(pageN: number): Page {
        if (pageN > this.pageCount || pageN < 0) {
            throw new RangeError("pageN out of range")
        }
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        const page: Page = this._instance.getPage(pageN)
        return new Page(page);
    }

    getObjects(): Array<Obj> {
        const objects: Array<any> = this._instance.getObjects()
        return objects.map(value => {
            return new Obj(value)
        })
    }

    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     * @param password
     */
    mergeDocument(doc: string, password?: string): void {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        password !== null ? this._instance.mergeDocument(doc, password) : this._instance.mergeDocument(doc)
    }

    deletePage(pageIndex: number): void {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        if (this.pageCount < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range')
        }
        this._instance.deletePage(pageIndex)
    }

    getVersion(): number {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getVersion()
    }

    isLinearized(): boolean {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.isLinearized()
    }


    /**
     * Persist changes and write to disk or if no arguments provided returns Buffer
     * @param {string} [file] - optional, if provided, will try to write to file
     */
    write(file?: string): Promise<string|Buffer> {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        if(file) {
            if(existsSync(file))
                console.warn(`File at ${file} will be overwritten`)
            return this._instance.write(file) as Promise<string>
        } else {
            console.warn('persisting pdf file changes to buffer')
            return this._instance.write() as Promise<Buffer>
        }
    }

    createEncrypt(option: EncryptInitOption): Encrypt {
        const encryptInstance = this._instance.createEncrypt(option)
        return new Encrypt(encryptInstance)
    }

    getTrailer(): Obj {
        let objInit =  this._instance.getTrailer()
        return new Obj(objInit)
    }

    freeObjMem(target:Obj): void {
        try {
            this._instance.freeObjMem(target._instance)
        } catch(e) {
            throw e
        }
    }
}