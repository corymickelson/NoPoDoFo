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
import { Data } from './data'
import { IDocument, NPDFFontEncoding, NPDFPageLayout, NPDFPageMode } from './document'
import {__mod as mod} from './base-document'
import { IStreamDocument, NPDFWriteMode, NPDFVersion } from './stream-document'
import { IObj } from './object'
import { Page } from './page'
import {
    Annotation,
    NPDFAction,
    NPDFAnnotation,
    NPDFAnnotationFlag,
    NPDFAnnotationType
} from './annotation'
import {
    CheckBox,
    Field,
    TextField,
    ListBox,
    ComboBox
} from './field'
import { Image } from './image'
import {
    Painter,
    Encoding,
    ExtGState,
    Font,
    NPDFAlignment,
    NPDFBlendMode,
    NPDFColorSpace,
    NPDFFontType,
    NPDFLineCapStyle,
    NPDFLineJoinStyle,
    NPDFRenderingIntent,
    NPDFStokeStyle,
    NPDFTextRenderingMode,
    NPDFVerticalAlignment
} from './painter'
import { Rect } from './rect'
import { Signer, signature } from './signer'
import { Stream } from './stream'
import { IForm } from './form'
import { ContentsTokenizer } from './parser'
import { IRef } from './reference'
import { Cell, Table } from './table'


export {
    ContentsTokenizer,
    IForm,
    Stream,
    Rect,
    Image,
    Data,
    IDocument,
    NPDFFontEncoding,
    IObj,
    Page,
    Annotation,
    NPDFAction,
    NPDFAnnotation,
    NPDFAnnotationFlag,
    NPDFAnnotationType,
    CheckBox,
    Field,
    TextField,
    ListBox,
    ComboBox,
    Painter,
    Encoding,
    ExtGState,
    Font,
    NPDFAlignment,
    NPDFBlendMode,
    NPDFColorSpace,
    NPDFFontType,
    NPDFLineCapStyle,
    NPDFLineJoinStyle,
    NPDFRenderingIntent,
    NPDFStokeStyle,
    NPDFTextRenderingMode,
    NPDFVerticalAlignment,
    Signer,
    signature,
    IRef,
    Cell,
    Table,
    mod,
    NPDFPageLayout,
    NPDFPageMode,
    IStreamDocument,
    NPDFWriteMode,
    NPDFVersion
}
export const CONVERSION = 0.0028346456693
export type NPDFExternal<T> = Object



