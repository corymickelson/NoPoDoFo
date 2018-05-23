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
<<<<<<< HEAD
import {Data} from './data'
import {Document, NPDFFontEncoding, __mod} from './document'
import {IObj} from './object'
import {Page} from './page'
=======
import { Data } from './data'
import { Document, FontEncoding, PageLayout, PageMode } from './document'
import { StreamDocument, NPDFWriteMode, NPDFVersion } from './stream-document'
import { Obj } from './object'
import { Page } from './page'
>>>>>>> feature/base-document
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
<<<<<<< HEAD
import {Rect} from './rect'
import {Signer, signature} from './signer'
import {Stream} from './stream'
import {IForm} from './form'
import {ContentsTokenizer} from './parser'
import {IRef} from './reference'
import {Cell, Table} from './table'
=======
import { Rect } from './rect'
import { Signer, signature } from './signer'
import { Stream } from './stream'
import { Form } from './form'
import { ContentsTokenizer } from './parser'
import { Ref } from './reference'
import { Cell, Table } from './table'
>>>>>>> feature/base-document


export {
    ContentsTokenizer,
    IForm,
    Stream,
    Rect,
    Image,
    Data,
    Document,
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
<<<<<<< HEAD
    __mod
=======
    PageLayout,
    PageMode,
    StreamDocument,
    NPDFWriteMode,
    NPDFVersion
>>>>>>> feature/base-document
}
export const CONVERSION = 0.0028346456693


