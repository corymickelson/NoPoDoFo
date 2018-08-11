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

import {IDocument} from "./document"
import {IDictionary, IObj} from "./object";

export enum NPDFActions {
    GoTo = 0,
    GoToR,
    GoToE,
    Launch,
    Thread,
    URI,
    Sound,
    Movie,
    Hide,
    Named,
    SubmitForm,
    ResetForm,
    ImportData,
    JavaScript,
    SetOCGState,
    Rendition,
    Trans,
    GoTo3DView,
    RichMediaExecute,
    Unknown = 0xff
}
export enum NPDFMouseEvent {
    up,
    down,
    enter,
    exit
}
export enum NPDFPageEvent {
    open,
    close,
    visible,
    invisible
}

/**
 * An Action can be used in conjunction with an outline to create bookmarks on the pdf
 * Action can also used to link annotatoins to external sources, as well as run scripts.
 * @see IOutline
 * @see IAnnotation
 */
export interface IAction {
    new(type:NPDFActions, doc:IDocument): void
    type: NPDFActions
    uri?: string
    script?: string

    getObject(): IObj
    addToDictionary(dictionary: IDictionary): void
}