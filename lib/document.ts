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
import {Obj} from './object';
import {Page} from './page';
import {Encrypt, EncryptOption, ProtectionOption} from './encrypt';
import {EventEmitter} from 'events';
import {Font} from "./painter";
import {Signer} from './signer';

export const __mod = require('bindings')('npdf')

export enum FontEncoding {
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

export interface CreateFontOpts {
    fontName: string,
    bold?: boolean,
    italic?: boolean,
    encoding?: FontEncoding,
    embed?: boolean,
    fileName?: string
}



/**
 * @class Document
 * @desc Document represents a PdfMemDocument, construct from an existing pdf document.
 * Document is the core class for reading and manipulating PDF files and writing them back to disk.
 * Document was designed to allow easy access to the object structure of a PDF file.
 * Document should be used whenever you want to change the object structure of a PDF file.
 */
export class Document extends EventEmitter {

    private _instance: any
    private _loaded: boolean = false;
    private _password: string | undefined = undefined

    get loaded() {
        return this._loaded
    }

    set password(value: string) {
        this._password = value;
    }

    get password(): string {
        throw TypeError('Passwords may not be retrieved after they\'ve been set on a document. For more information see Encrypt')
    }

    get encrypt(): Encrypt | null {
        if(this._instance.hasEncrypt()) {
            return new Encrypt(this)
        } else return null
    }


    /**
     * File is loaded asynchronously, extends eventEmitter, will publish a 'ready'event when document has been loaded
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @param update
     * @returns void
     */
    constructor(file: string|Buffer, update: boolean = false) {
        super()
        this._instance = new __mod.Document()
        if (typeof(file) === 'string') {
            access(file, constants.F_OK | constants.R_OK, err => {
                if (err){
                    this.emit('error', Error('file not found'))
                } else {
                    this.load(file, update)
                }
            })
        } else if(Buffer.isBuffer(file)) {
            this.load(file, update)
        }
    }

    /**
     * load pdf file, emits 'ready' || 'error' events
     * @param file - file path
     * @param update - load document for incremental updates
     */
    private load(file: string | Buffer, update: boolean = false): void {
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
        if (pageN > this.getPageCount() || pageN < 0) {
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
        if (this.getPageCount() < pageIndex || pageIndex < 0) {
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

    /**
     * @desc Creates a PdfFont instance for use in NoPoDoFo generated Pdf Document. Note
     *      it is up to the user to check that the specified font family exists on the system.
     *      For font management use font-manager
     * @see https://github.com/corymickelson/font-manager
     * @param {CreateFontOpts & Object} opts
     * @returns {Font}
     */
    createFont(opts: CreateFontOpts & Object): Font {
        const instance = this._instance.createFont(
            opts.fontName,
            opts.hasOwnProperty('bold') ? opts.bold : false,
            opts.hasOwnProperty('italic') ? opts.italic : false,
            opts.hasOwnProperty('encoding') ? opts.encoding : 1,
            opts.hasOwnProperty('embed') ? opts.embed : false,
            opts.hasOwnProperty('fileName') ? opts.fileName : null)
        return new Font(instance)
    }

    writeUpdate(device: string | Signer): void {
        if (device instanceof Signer)
            this._instance.writeUpdate((device as any)._instance)
        else this._instance.writeUpdate(device)
    }

    createEncrypt(opts: EncryptOption): Encrypt {
        this._instance.encrypt = opts
        if(this.encrypt === null) {
            throw Error('Failed to set encrypt')
        }
        return this.encrypt as Encrypt
    }
}