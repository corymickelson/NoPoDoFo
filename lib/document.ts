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
import { access } from 'fs'
import { IObj } from './object';
import { IEncrypt } from './encrypt';
import { IFont } from "./painter";
import { F_OK } from "constants";
import { IBase } from './base-document'
import {ISignatureField, npdf} from '.';
import {Callback} from "./index";


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
            password?: string
        },
        cb: Callback<void>): void
    load(file: string | Buffer, cb: Callback<void>): void

    /**
     * Deletes one or more pages from the document by removing the pages reference
     * from the pages tree. This does NOT remove the page object as the page object
     * may be used by other objects in the document.
     * @param startIndex - first page to delete (0-based)
     * @param count - number of pages to delete
     */
    splicePages(startIndex: number, count: number): void

    /**
     * Copies one or more pages from another pdf to this document
     * This function copies the entire document to the target document and then
     * deletes pages that are not of interest. This is a much faster process, but
     * without object garbage collection the document may result in a much larger
     * than necessary document
     * @see {IDocument#gc}
     * @param fromDoc - PdfMemDocument to append
     * @param startIndex - first page to copy (0-based)
     * @param count - number of pages to copy
     */
    insertPages(fromDoc: IDocument, startIndex: number, count: number): number

    /**
     * Persist the document with any changes applied to either a new nodejs buffer or to disk.
     * @param destination - file path or callback function
     * @param cb - if file path was provided as destination, this must be a callback function
     */
    write(destination: Callback<Buffer> | string, cb?: Callback<string>): void

    /**
     * Looks for a font with the provided name or id.
     * This operation may be expensive as it iterates all pdf object searching for the font.
     * If nothing is found null is returned
     * @param name - font name or font id
     */
    getFont(name: string): IFont

    /**
     * List all the fonts from the document.
     */
    listFonts(): { id: string, name: string }[]

    /**
     * Performs garbage collection on the document. All objects not
     * reachable by the trailer or deleted.
     * @param file - pdf document
     * @param pwd - if document is password protected this parameter is required
     * @param output - file location
     * @param cb - function
     */
    gc(file: string, pwd: string, output: string, cb: Callback<string|Buffer>): void

    /**
     * Check for the existence of a signature field(s)
     * @returns {boolean}
     */
    hasSignatures(): boolean

    /**
     * Iterate each page annotations array for an annotation of type signature.
     * @returns {ISignatureField[]}
     */
    getSignatures(): ISignatureField[]
}

export const documentGc = (file: string, pwd: string, output: string, cb: Callback<string|Buffer>) => {
    access(file, F_OK, err => {
        if (err) {
            throw Error('File not found')
        }
        (npdf.Document as any).gc(file, pwd, output, cb) // gc is a static method on Document
    })
}



