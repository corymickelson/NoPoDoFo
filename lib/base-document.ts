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
import { EventEmitter } from "events";
import { NPDFInternal, IObj } from "./object";
import {IEncrypt, ProtectionOption} from "./encrypt";
import {NPDFPageMode, NPDFPageLayout, NPDFCreateFontOpts, IDocument, Callback} from "./document";
import { IRef } from "./reference";
import {IPage, Page} from "./page";
import {Font, IFont} from "./painter";
import { access } from "fs";
import { R_OK, F_OK } from "constants";
import {IForm} from "../dist";
import {NPDFVersion, NPDFWriteMode} from "./stream-document";
import {Rect} from "./rect";
export const __mod = require('bindings')('npdf')

export interface NPDFInfo {
    author:string
    createdAt:Date
    creator:string
    keywords:string
    producer:string
    subject:string
    title:string
}

export enum NPDFDestinationFit {
    Fit,
    FitH,
    FitV,
    FitB,
    FitBH,
    FitBV,
    Unknown = 0xFF
}

export interface IBase {
    readonly form: IForm
    readonly body: IObj[]
    readonly version: NPDFVersion
    pageMode: NPDFPageMode
    pageLayout: NPDFPageLayout
    printingScale: string
    baseUri: string
    language: string
    readonly info: NPDFInfo

    getPageCount(): number
    getPage(n: number): IPage
    hideToolbar(): void
    hideMenubar(): void
    hideWindowUI(): void
    fitWindow(): void
    centerWindow(): void
    displayDocTitle(): void
    useFullScreen(): void
    attachFile(file:string, document:IBase): void
    insertExistingPage(memDoc:IDocument, index: number): number
    insertPage(rect: Rect, index:number): IPage
    append(doc: string|IDocument): void
    isLinearized(): boolean
    getWriteMode():NPDFWriteMode
    getObject(ref: IRef): IObj
    isAllowed(perm: ProtectionOption): boolean
    createFont(opts: NPDFCreateFontOpts): IFont
    getOutlines(): IObj
    getNames(): IObj
    createPage(rect: Rect): IPage
    createPages(rects: Rect[]): number
    getAttachment(uri: string): IFileSpec
    addDestination(page:IPage, destination: NPDFDestinationFit, name:string): void
     
    // debugging helper functions
    __ptrCount(): number // Get the number of shared_ptr<PdfDocument> in use.
}
export interface IFileSpec {
    readonly name: string
}

export class BaseDocument extends EventEmitter {
    private _base: NPDFInternal
    private _encrypt?: IEncrypt
    private _password?: string

    get base(): NPDFInternal {
        return this._base
    }

    set password(value: string) {
        this._password = value;
    }

    get password(): string {
        throw EvalError()
    }

    set pageMode(v: NPDFPageMode) {
        this._base.pageMode = v
    }

    get pageMode(): NPDFPageMode {
        return this._base.pageMode
    }

    set pageLayout(v: NPDFPageLayout) {
        this._base.pageLayout = v
    }

    set printingScale(v: string) {
        this._base.printingScale = v
    }

    set baseURI(v: string) {
        this._base.baseURI = v
    }

    set language(v: string) {
        this._base.language = v
    }

    get writeMode(): NPDFWriteMode {
        return this.base.writeMode as NPDFWriteMode
    }
    /**
     * @todo: refactor internal implementation to use enum
     */
    get version(): NPDFVersion {
        return this.base.version as NPDFVersion
    }

    get info(): NPDFInfo {
        return this._base.info
    }
    get encrypt(): IEncrypt {
        if (this._encrypt) return this._encrypt
        else {
            const encrypt = new __mod.Encrypt(this._base)
            this._encrypt = encrypt
            return encrypt
        }
    }
    constructor(file?: string, createStream: boolean = false) {
        super()
    }
    /**
     * @todo Fix this, should not have to pass internal to Base Class
     * @param instance - document instance
     */
    setInternal(instance: NPDFInternal): void {
        this._base = instance
    }
    getPageCount(): number {

        return this._base.getPageCount()
    }

    getPage(pageN: number): Page {
        if (pageN > this.getPageCount() || pageN < 0) {
            throw new RangeError("pageN out of range")
        }

        const page: Page = this._base.getPage(pageN)
        return new Page(page);
    }

    getObject(ref: IRef): IObj {
        if (!ref || ref instanceof (__mod.Ref as any) === false) {
            throw TypeError()
        }
        else if (ref.isIndirect() === false) {
            throw Error('Document.GetObject is only possible when the object referenced is an indirect object')
        }
        return this._base.getObject((ref as any)._base)
    }

    isLinearized(): boolean {
        return this._base.isLinearized()
    }
    /**
     * @desc Creates a PdfFont instance for use in NoPoDoFo generated Pdf Document. Note
     *      it is up to the user to check that the specified font family exists on the system.
     *      For font management use font-manager
     * @see https://github.com/corymickelson/font-manager
     * @param {CreateFontOpts & Object} opts
     * @returns {Font}
     */
    createFont(opts: NPDFCreateFontOpts & Object): Font {
        const instance = this._base.createFont(
            opts.fontName,
            opts.hasOwnProperty('bold') ? opts.bold : false,
            opts.hasOwnProperty('italic') ? opts.italic : false,
            opts.hasOwnProperty('encoding') ? opts.encoding : 1,
            opts.hasOwnProperty('embed') ? opts.embed : false,
            opts.hasOwnProperty('fileName') ? opts.fileName : null)
        return new Font(instance)
    }

    attachFile(file: string): Promise<void> {
        return new Promise((resolve, reject) => {
            access(file, F_OK | R_OK, err => {
                if (err) {
                    return reject('File Not Found')
                }
                else {
                    this._base.attachFile(file)
                    return resolve()
                }
            })

        })
    }

    insertExistingPage(doc:IDocument, docIndex:number, atIndex:number):number {
        return this.base.getPageCount()
    }

    getOutlines(create:boolean = false):IObj {
        return this._base.getOutlines()
    }

    getNamesTree(create:boolean = false): IObj {
        return this._base.getNames()
    }

    createPage(opt:Rect):IPage {
        return this._base.createPage(opt)
    }
    createPages(opts:Rect[]):number {
        return this._base.createPages(opts)
    }
    insertPage(opt:Rect, at:number):IPage {
        return this._base.insertExistingPage(opt, at)
    }

}