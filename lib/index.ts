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
import { Document as doc } from './document'
import { Obj as obj } from './object'
import { Page as page } from './page'
import {
    Annotation as annot,
    NPDFAction as action,
    NPDFAnnotation as annotenum,
    NPDFAnnotationFlag as annotflags,
    NPDFAnnotationType as annottype
} from './annotation'
import {
    CheckBox as checkbox,
    Field as field,
    TextField as textfield,
    ListBox as listbox,
    ComboBox as combobox
} from './field'
import { Image as image } from './image'
import { Painter as painter } from './painter'
import { Rect as rect } from './rect'
import { Signer as sign } from './signer'
import {Stream as stream} from './stream'
import {Form as form} from './form'
import {ContentsTokenizer as ct} from './parser'

export const Document = doc,
    Page = page,
    Obj = obj,
    Annotation = annot,
    NPdfAction = action,
    NPdfAnnotation = annotenum,
    NPdfAnnotationFlag = annotflags,
    NPdfAnnotationType = annottype,
    Field = field,
    Form = form,
    CheckBox = checkbox,
    TextField = textfield,
    ListBox = listbox,
    ComboBox = combobox,
    Image = image,
    Painter = painter,
    Rect = rect,
    Signature = sign,
    Stream = stream,
    ContentsTokenizer = ct,
    CONVERSION = 0.0028346456693

