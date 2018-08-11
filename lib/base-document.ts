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
import {IObj, Ref} from "./object";
import { ProtectionOption } from "./encrypt";
import { NPDFPageMode, NPDFPageLayout, NPDFCreateFontOpts, IDocument } from "./document";
import { IPage } from "./page";
import { IFont } from "./painter";
import { IForm } from "./form";
import { IRect } from "./rect";
import { IXObj } from ".";
import {IOutline} from "./outlines";
import {NPDFDestinationFit} from './destination'
import {IFileSpec} from "./file-spec";

export interface NPDFInfo {
    author: string
    createdAt: Date
    creator: string
    keywords: string
    producer: string
    subject: string
    title: string
}

export enum NPDFVersion {
    Pdf11,
    Pdf12,
    Pdf13,
    Pdf14,
    Pdf15,
    Pdf16,
    Pdf17,
}

export enum NPDFWriteMode {
    Default = 0x01,
    Compact = 0x02
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
    attachFile(file: string): void
    insertExistingPage(memDoc: IDocument, index: number, insertIndex: number): number
    insertPage(rect: IRect, index: number): IPage
    append(doc: IDocument): void
    isLinearized(): boolean
    getWriteMode(): NPDFWriteMode
    isAllowed(perm: ProtectionOption): boolean
    createFont(opts: NPDFCreateFontOpts): IFont

    /**
     * Get an existing outline or create a new outline and new root node
     * @param {boolean} [create] - Create a new outline if one does not already exist
     * @param {string} [root] - Create a Root node with the provided name
     */
    getOutlines(create?: boolean, root?: string): null | IOutline
    getObject(ref: Ref): IObj
    getNames(create: boolean): IObj|null
    createXObject(rect: IRect): IXObj
    createPage(rect: IRect): IPage
    createPages(rects: IRect[]): number
    getAttachment(uri: string): IFileSpec
    addNamedDestination(page: IPage, destination: NPDFDestinationFit, name: string): void
}


