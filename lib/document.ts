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
import {access, constants} from 'fs'
import {IArray, IDictionary, IObj} from './object';
import {Page, IPage} from './page';
import {EncryptOption, IEncrypt, ProtectionOption} from './encrypt';
import {Font, IFont} from "./painter";
import {Signer} from './signer';
import {F_OK, R_OK} from "constants";
import {IRef} from "./reference";
import {IForm} from "./form";
import {BaseDocument, IBase} from './base-document'
import {ICheckBox, IComboBox, IField, IListBox, IListField, ISignatureField, ITextField} from "./field";
import {IAction} from "./action";
import {IStreamDocument} from "./stream-document";
import {IImage} from "./image";
import {IAnnotation} from "./annotation";
import {IFileSpec} from "./file-spec";
import {IContentsTokenizer} from "./parser";
import {IRect} from "./rect";

export declare enum NPDFVersion {
    Pdf11 = 0,
    Pdf12 = 1,
    Pdf13 = 2,
    Pdf14 = 3,
    Pdf15 = 4,
    Pdf16 = 5,
    Pdf17 = 6,
}

export declare enum NPDFWriteMode {
    Default = 1,
    Compact = 2,
}

export interface INPDF {
    Document: IDocument
    StreamDocument: IStreamDocument
    Page: IPage
    Field: IField
    TextField: ITextField
    Image: IImage
    Annotation: IAnnotation
    Rect: IRect
    Painter: any
    CheckBox: ICheckBox
    ComboBox: IComboBox
    ListBox: IListBox
    Form: IForm
    Dictionary: IDictionary
    FileSpec: IFileSpec
    Obj: IObj
    Ref: IRef
    Array: IArray
    Stream: any
    Encrypt: IEncrypt
    ListField: IListField
    Font: IFont
    Encoding: any
    ExtGState: any
    Signer: any
    SignatureField: ISignatureField
    Data: any
    ContentsTokenizer: IContentsTokenizer
    SimpleTable: any
    Action: IAction
    signature: Function
}

export const __mod: INPDF = require('bindings')('npdf')
export type Callback = (err: Error, data: Buffer | string) => void

export enum NPDFFontEncoding {
    WinAnsi = 1,
    Standard = 2,
    PdfDoc = 3,
    MacRoman = 4,
    MacExpert = 5,
    Symbol = 6,
    ZapfDingbats = 7,
    Win1250 = 8,
    Iso88592 = 9,
    Identity = 0
}

export enum PageMode {
    DontCare,
    FullScreen,
    UseAttachments,
    UseBookmarks,
    UseNone,
    UseOC,
    UseThumbs
}

export enum PageLayout {
    Ignore,
    Default,
    SinglePage,
    OneColumn,
    TwoColumnLeft,
    TwoColumnRight,
    TwoPageLeft,
    TwoPageRight
}

export interface NPDFCreateFontOpts {
    fontName: string,
    bold?: boolean,
    italic?: boolean,
    encoding?: NPDFFontEncoding,
    embed?: boolean,
    fileName?: string
}

export interface IDocument extends IBase {
    new(): IDocument

    password: string
    encrypt: IEncrypt
    readonly trailer: IObj
    readonly catalog: IObj

    load(file: string | Buffer,
         opts: {
             forUpdate?: boolean,
             fromBuffer?: boolean,
             password?: string
         } | Callback,
         cb?: Callback): void

    splicePages(startIndex: number, count: number): void

    write(destination: Callback | string, cb: Callback): void

    getFont(name: string): IFont
}

export const documentGc = (file: string, pwd: string, output: string, cb: Callback) => {
    access(file, F_OK, err => {
        if (err) {
            throw Error('File not found')
        }
        (__mod.Document as any).gc(file, pwd, output, cb) // gc is a static method on Document
    })
}


/**
 * @class Document
 * @desc Document represents a PdfMemDocument, construct from an existing pdf document.
 * Document is the core class for reading and manipulating PDF files and writing them back to disk.
 * Document was designed to allow easy access to the object structure of a PDF file.
 * Document should be used whenever you want to change the object structure of a PDF file.
 */
export class Document extends BaseDocument {
    private readonly _instance: any
    private _loaded: boolean = false;

    get body(): IObj[] {
        return this._instance.body
    }

    get trailer(): IObj {
        return this._instance.trailer
    }

    get catalog(): IObj {
        return this._instance.catalog
    }

    get version(): number {
        return this._instance.version
    }

    /**
     * @desc A Document has been read into memory
     * @returns {boolean}
     */
    get loaded() {
        return this._loaded
    }

// <<<<<<< HEAD
//     /**
//      * @description If the document has an AcroForm Dictionary return the form as an instance of IForm.
//      *      If there is not an AcroForm Dictionary for the document, doing a get on form will create an new
//      *      empty AcroForm Dictionary.
//      * @todo: Add configuration to disable creation of new form on form getter.
//      */
//     get form(): IForm {
//         return this._instance.form
//     }
//
    set password(value: string) {
        this._instance.password = value
    }

    get password(): string {
        throw EvalError('Password is not a retrievable property')
    }

//
//     get encrypt(): IEncrypt {
//         if (this._encrypt) return this._encrypt
//         else {
//             const encrypt = new __mod.Encrypt(this._instance)
//             this._encrypt = encrypt
//             return encrypt
//         }
//     }
//
// =======
// >>>>>>> feature/base-document
    static gc(file: string, pwd: string, output: string, cb: (e: Error, d: string | Buffer) => void): void {
        access(file, F_OK, err => {
            if (err) {
                throw Error('File not found')
            }
            (__mod.Document as any).gc(file, pwd, output, cb) // gc is a static method on mod.Document
        })
    }

    /**
     * File is loaded asynchronously, extends eventEmitter, will publish a 'ready'event when document has been loaded
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @param update
     * @param {string} [pwd] - document password
     * @returns void
     */
    constructor(file: string | Buffer, update: boolean = false, pwd?: string) {
        super(undefined, false)
        // this._instance = new __mod.Document()
        // this.setInternal(this._instance)
        if (Buffer.isBuffer(file)) {
            this.load(file, update, pwd || '')
        } else {
            access(file, constants.F_OK | constants.R_OK, err => {
                if (err) {
                    this.emit('error', Error('file not found'))
                } else {
                    this.load(file, update, pwd || '')
                }
            })
        }
    }

    /**
     * @desc load pdf file, emits 'ready' || 'error' events
     * @param file - file path
     * @param update - load document for incremental updates
     * @param pwd
     */
    private load(file: string | Buffer, update: boolean = false, pwd?: string): void {
        let cb = (e: Error) => {
            if (e) {
                if (e.message === "Password required to modify this document" && pwd) {
                    try {
                        this.password = pwd
                        this._loaded = true
                        this.emit('ready', this)
                    } catch (e) {
                        this.emit('error', e)
                    }
                } else {
                    this.emit('error', e)
                }
            } else {
                this._loaded = true
                this.emit('ready', this)
            }
        }
        this._instance.load(file, cb, update, Buffer.isBuffer(file), pwd || '')
    }

//
// <<<<<<< HEAD
//     getPageCount(): number {
//         if (!this._loaded) {
//             throw new Error('load a pdf file before calling this method')
//         }
//         return this._instance.getPageCount()
//     }
//
//     getPage(pageN: number): Page {
//         if (pageN > this.getPageCount() || pageN < 0) {
//             throw new RangeError("pageN out of range")
//         }
//         if (!this._loaded) {
//             throw new Error('load a pdf file before calling this method')
//         }
//         const page: Page = this._instance.getPage(pageN)
//         return new Page(page);
//     }
//
//     /**
//      * @desc Get an NoPoDoFo Obj from an indirect reference
//      * @param {IRef} ref
//      * @returns {IObj}
//      */
//     getObject(ref: IRef): IObj {
//         if (!ref || (ref as any)._instaance instanceof __mod.Ref === false) {
//             throw TypeError()
//         }
//         else if (ref.isIndirect() === false) {
//             throw Error('Document.GetObject is only possible when the object referenced is an indirect object')
//         }
//         return this._instance.getObject((ref as any)._instance)
//     }
//
// =======
// >>>>>>> feature/base-document
    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     * @param password
     * @returns {Promise}
     */
    appendDocument(doc: string, password?: string): Promise<any> {
        return new Promise((resolve, reject) => {
            if (!this._loaded) {
                reject(new Error('load a pdf file before calling this method'))
            }
            access(doc, F_OK | R_OK, err => {
                if (err) return reject(err)
                else {
                    try {
                        password !== null ? this._instance.appendDocument(doc, password) : this._instance.appendDocument(doc)
                        return resolve()
                    } catch (err) {
                        return reject(err)
                    }
                }
            })
        })

    }

    splicePage(pageIndex: number): void {
        if (!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        if (this.getPageCount() < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range')
        }
        this._instance.splicePage(pageIndex)
    }

    /**
     * Persist changes and write to disk or if no arguments provided returns Buffer
     * @param {string|Function} output - optional, if provided, will try to write to file
     * @param {Function} [cb] - optional callback
     */
    write(output: Callback | string, cb?: Callback): void {
        if (!this._loaded) {
            throw Error('Document has not been loaded, await ready event')
        }
        if (typeof output === 'string' && cb !== null || cb !== undefined) {
            this._instance.write(output, cb)
        } else {
            this._instance.writeBuffer(output)
        }
    }

    isAllowed(protection: ProtectionOption): boolean {
        return this._instance.isAllowed(protection)
    }

// <<<<<<< HEAD
//     /**
//      * @desc Creates a PdfFont instance for use in NoPoDoFo generated Pdf Document. Note
//      *      it is up to the user to check that the specified font family exists on the system.
//      *      For font management use font-manager
//      * @see https://github.com/corymickelson/font-manager
//      * @param {CreateFontOpts & Object} opts
//      * @returns {Font}
//      */
//     createFont(opts: CreateFontOpts & Object): IFont {
//         return this._instance.createFont(
//             opts.fontName,
//             opts.hasOwnProperty('bold') ? opts.bold : false,
//             opts.hasOwnProperty('italic') ? opts.italic : false,
//             opts.hasOwnProperty('encoding') ? opts.encoding : 1,
//             opts.hasOwnProperty('embed') ? opts.embed : false,
//             opts.hasOwnProperty('fileName') ? opts.fileName : null)
//     }
//
// =======
// >>>>>>> feature/base-document
    writeUpdate(device: string | Signer): void {
        if (device instanceof Signer)
            this._instance.writeUpdate((device as any)._instance)
        else this._instance.writeUpdate(device)
    }

    createEncrypt(opts: EncryptOption): IEncrypt {
        this._instance.encrypt = opts
        if (this.encrypt === null) {
            throw Error('Failed to set encrypt')
        }
        return this.encrypt as IEncrypt
    }

    getFont(identifier: string): IFont | null {
        return this._instance.getFont(identifier)
    }
}