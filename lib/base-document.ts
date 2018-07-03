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
import { IObj } from "./object";
import {ProtectionOption} from "./encrypt";
import {NPDFPageMode, NPDFPageLayout, NPDFCreateFontOpts, IDocument} from "./document";
import {IPage} from "./page";
import {IFont} from "./painter";
import {IForm} from "./form";
import {NPDFVersion, NPDFWriteMode} from "./stream-document";
import {IRect} from "./rect";
import { IXObj } from ".";

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
    insertExistingPage(memDoc:IDocument, index: number, insertIndex: number): number
    insertPage(rect: IRect, index:number): IPage
    append(doc: string|IDocument): void
    isLinearized(): boolean
    getWriteMode():NPDFWriteMode
    isAllowed(perm: ProtectionOption): boolean
    createFont(opts: NPDFCreateFontOpts): IFont
    getOutlines(): IObj
    getNames(): IObj
    createXObject(rect: IRect): IXObj
    createPage(rect: IRect): IPage
    createPages(rects: IRect[]): number
    getAttachment(uri: string): IFileSpec
    addDestination(page:IPage, destination: NPDFDestinationFit, name:string): void
     
    // debugging helper functions
    __ptrCount(): number // Get the number of shared_ptr<PdfDocument> in use.
}
export interface IFileSpec {
    readonly name: string
}

