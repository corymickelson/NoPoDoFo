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
import {IPage} from './page';
import {EncryptOption, IEncrypt, ProtectionOption} from './encrypt';
import {IEncoding, IExtGState, IFont, IPainter} from "./painter";
import {ISigner} from './signer';
import {F_OK, R_OK} from "constants";
import {IForm} from "./form";
import {IBase} from './base-document'
import {ICheckBox, IComboBox, IField, IListBox, IListField, ISignatureField, ITextField} from "./field";
import {IAction} from "./action";
import {IStreamDocument} from "./stream-document";
import {IImage} from "./image";
import {IAnnotation} from "./annotation";
import {IFileSpec} from "./file-spec";
import {IContentsTokenizer} from "./parser";
import {IRect} from "./rect";
import {IData} from "./data";

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
    Painter: IPainter
    CheckBox: ICheckBox
    ComboBox: IComboBox
    ListBox: IListBox
    Form: IForm
    Dictionary: IDictionary
    FileSpec: IFileSpec
    Obj: IObj
    Array: IArray
    Stream: any
    Encrypt: IEncrypt
    ListField: IListField
    Font: IFont
    Encoding: IEncoding
    ExtGState: IExtGState
    Signer: ISigner
    SignatureField: ISignatureField
    Data: IData
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

export enum NPDFPageMode {
    DontCare,
    UseNone,
    UseThumbs,
    UseBookmarks,
    FullScreen,
    UseOC,
    UseAttachments
}

export enum NPDFPageLayout {
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

/**
 * @interface IDocument
 * @desc Document represents a PdfMemDocument, construct from an existing pdf document.
 * Document is the core class for reading and manipulating PDF files and writing them back to disk.
 * Document was designed to allow easy access to the object structure of a PDF file.
 * Document should be used whenever you want to change the object structure of a PDF file.
 */
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
         },
         cb: Callback): void
    load(file: string | Buffer, cb: Callback): void

    splicePages(startIndex: number, count: number): void

    write(destination: Callback | string, cb?: Callback): void

    getFont(name: string): IFont

    gc(file: string, pwd: string, output: string, cb: Callback): void
}

export const documentGc = (file: string, pwd: string, output: string, cb: Callback) => {
    access(file, F_OK, err => {
        if (err) {
            throw Error('File not found')
        }
        (__mod.Document as any).gc(file, pwd, output, cb) // gc is a static method on Document
    })
}



