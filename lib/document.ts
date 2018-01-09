import {exists, existsSync} from 'fs'
import {Obj} from './object';
import {Page} from './page';
import {Encrypt, EncryptOption, ProtectionOption} from './encrypt';
import {EventEmitter} from 'events';
import {Font} from "./painter";
import { Signer } from './signer';

export const __mod = require('bindings')('npdf')
export enum FontEncoding {
    WinAnsi = 1,
    Standard = 2,
    PdfDoc = 3,
    MacRoman= 4,
    MacExpert = 5,
    Symbol = 6,
    ZapfDingbats = 7,
    Win1250 = 8,
    Iso88592 = 9,
    Identity = 0
}
export interface CreateFontOpts {
    fontName:string,
    bold?:boolean,
    italic?:boolean,
    encoding?: FontEncoding,
    embed?:boolean,
    fileName?:string
}
export interface IDocument {
    encrypt: Encrypt

    load(file: string, update?: boolean): void

    getPageCount(): number

    getPage(pageN: number): Page

    getObjects(): Array<Obj>

    mergeDocument(doc: string): void

    deletePage(pageIndex: number): void

    getVersion(): number

    isLinearized(): boolean

    write(cb: (e: Error) => void, file?: string): void

    writeUpdate(device:string|Signer): void  

    getTrailer(): Obj

    isAllowed(protection: ProtectionOption): boolean

    createFont(opts:CreateFontOpts): Font
}


/**
 * @class Document
 * @desc Document represents a PdfMemDocument, construct from an existing pdf document.
 * Document is the core class for reading and manipulating PDF files and writing them back to disk.
 * Document was designed to allow easy access to the object structur of a PDF file.
 * Document should be used whenever you want to change the object structure of a PDF file.
 */
export class Document extends EventEmitter implements IDocument {

    private _instance: any
    private _loaded: boolean = false;
    private _pageCount: number
    private _password: string

    get loaded() {
        return this._loaded
    }

    get pageCount() {
        return this._pageCount
    }

    set password(value: string) {
        this._password = value;
    }

    set encrypt(instance: Encrypt) {
        if (instance.option) this._instance.encrypt = instance.option
        else {
            throw Error("Set document encrypt with an instance of Encrypt with the optional EncryptInitOption defined at construction")
        }
    }

    get encrypt(): Encrypt {
        const instance = this._instance.encrypt
        return new Encrypt(instance)
    }

    /**
     * File is loaded asynchronously, extends eventEmitter, will publish a 'ready'event when document has been loaded
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @param update
     * @returns void
     */
    constructor(file: string, update: boolean = false) {
        super()
        this._instance = new __mod.Document()
        if (file) {
            if (!existsSync(file))
                throw new Error('file not found')
            this.load(file, update)
        }
    }

    /**
     * load pdf file, emits 'ready' || 'error' events
     * @param file - file path
     * @param update - load document for incremental updates
     */
    load(file: string | Buffer, update: boolean = false): void {
        this._instance.load(file, (e: Error) => {
            if (e) {
                this.emit('error', e)
            } else {
                this._loaded = true
                this.emit('ready', this)
            }
        }, update)
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
     * @param {Function} cb - Callbck Function
     */
    write(cb: (e: Error, v: Buffer | void) => void, file?: string): void {
        if (!this._loaded) {
            throw Error('Document has not been loaded, await ready event')
        }
        if (file) {
            this._instance.write(file, cb)
        } else {
            this._instance.writeBuffer(cb)
        }
    }

    getTrailer(): Obj {
        let objInit = this._instance.getTrailer()
        return new Obj(objInit)
    }

    isAllowed(protection: ProtectionOption): boolean {
        return this._instance.isAllowed(protection)
    }

    createFont(opts:CreateFontOpts & Object): Font {
        const instance = this._instance.createFont(
            opts.fontName,
            opts.hasOwnProperty('bold') ? opts.bold : false,
            opts.hasOwnProperty('italic') ? opts.italic : false,
            opts.hasOwnProperty('encoding') ? opts.encoding : 1,
            opts.hasOwnProperty('embed') ? opts.embed : false,
            opts.hasOwnProperty('fileName')? opts.fileName : null)
        return new Font(instance)
    }

    writeUpdate(device:string|Signer): void {
        if(device instanceof Signer)
            this._instance.writeUpdate((device as any)._instance)
        else this._instance.writeUpdate(device)
    }
}