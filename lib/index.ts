const buildType = process.config.target_defaults.default_configuration;
const npdf: INPDF = require(`../build/${buildType}/nopodofo.node`)
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
import { IData } from './data'
import { IDate, toPDFFormat } from './date'
import { IDocument, NPDFFontEncoding, NPDFPageLayout, NPDFPageMode } from './document'
import { IPage } from './page'
import { IStreamDocument, NPDFWriteMode, NPDFVersion } from './stream-document'
import {
    IObj, IDictionary, NPDFInternal, IArray, NPDFCoerceKeyType, NPDFDataType, NPDFDictionaryKeyType
} from './object'
import { IXObj } from './xobject'
import {
    IAnnotation,
    NPDFAction,
    NPDFAnnotation,
    NPDFAnnotationFlag,
    NPDFAnnotationType
} from './annotation'
import {
    ICheckBox,
    IField,
    ITextField,
    IListBox,
    IComboBox,
    IListField,
    IPushButton,
    ISignatureField,
    NPDFAnnotationAppearance,
    IListItem,
    NPDFCertificatePermission,
    NPDFFieldType,
    NPDFTextFieldOpts
} from './field'
import { IImage } from './image'
import {
    IPainter,
    IEncoding,
    IExtGState,
    IFont,
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
import { IRect } from './rect'
import { ISigner } from './signer'
import { Stream } from './stream'
import { IForm } from './form'
import { IContentsTokenizer } from './parser'
import { Cell, Table } from './table'
import { IEncrypt, ProtectionOption, EncryptOption, ProtectionSummary } from './encrypt'
import { NPDFName } from './names'
import { IFileSpec } from './file-spec';
import { IAction } from './action';
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
    Date: IDate
}

// const binary = require('node-pre-gyp')
// const { resolve, join } = require('path')
// const npdf: INPDF = require('bindings')('nopodofo.node')

// const __binary = binary.find(resolve(join(__dirname, '../package.json')))
// const npdf: INPDF = require(__binary)
export {
    IDate,
    IDictionary,
    NPDFInternal,
    IArray,
    NPDFCoerceKeyType,
    NPDFDataType,
    NPDFDictionaryKeyType,
    NPDFName,
    IListField,
    IPushButton,
    ISignatureField,
    NPDFAnnotationAppearance,
    IListItem,
    NPDFCertificatePermission,
    NPDFFieldType,
    NPDFTextFieldOpts,
    IEncrypt,
    ProtectionSummary,
    ProtectionOption,
    EncryptOption,
    IContentsTokenizer,
    IForm,
    Stream,
    IRect,
    IImage,
    IData,
    IDocument,
    NPDFFontEncoding,
    IObj,
    IPage,
    IAnnotation,
    NPDFAction,
    NPDFAnnotation,
    NPDFAnnotationFlag,
    NPDFAnnotationType,
    ICheckBox,
    IField,
    ITextField,
    IListBox,
    IComboBox,
    IPainter,
    IEncoding,
    IExtGState,
    IFont,
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
    ISigner,
    Cell,
    Table,
    npdf,
    NPDFPageLayout,
    NPDFPageMode,
    IStreamDocument,
    NPDFWriteMode,
    NPDFVersion,
    IXObj,
    toPDFFormat
}
export const CONVERSION = 0.0028346456693
/**
 * A generic type representing a n-api external pointer type
 * This can not be used in any way other than passing the external data type
 * back into napi as function argument.
 */
export type NPDFExternal<T> = Object


