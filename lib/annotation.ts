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
import {NPDFColor} from './painter';
import {IAction} from "./action";
import {IDestination} from './destination'
import {IFileSpec} from "./file-spec";
import { IXObj } from './xobject';

export enum NPDFAnnotationType {
    Text = 'Text',
    Link = 'Link',
    FreeText = 'FreeText',
    Line = 'Line',
    Square = 'Square',
    Circle = 'Circle',
    Polygon = 'Polygon',
    PolyLine = 'PolyLine',
    Highlight = 'Highlight',
    Underline = 'Underline',
    Squiggly = 'Squiggly',
    StrikeOut = 'StrikeOut',
    Stamp = 'Stamp',
    Caret = 'Caret',
    Ink = 'Ink',
    Popup = 'Popup',
    FileAttachment = 'FileAttachment',
    Sound = 'Sound',
    Movie = 'Movie',
    Widget = 'Widget',
    Screen = 'Screen',
    PrinterMark = 'PrinterMark',
    TrapNet = 'TrapNet',
    Watermark = 'Watermark',
    _3D = '3D',
    RichMedia = 'RichMedia',
    WebMedia = 'WebMedia'
}

export enum NPDFAnnotation {
    Text = 0,                   // - supported
    Link,                       // - supported
    FreeText,       // PDF 1.3  // - supported
    Line,           // PDF 1.3  // - supported
    Square,         // PDF 1.3
    Circle,         // PDF 1.3
    Polygon,        // PDF 1.5
    PolyLine,       // PDF 1.5
    Highlight,      // PDF 1.3
    Underline,      // PDF 1.3
    Squiggly,       // PDF 1.4
    StrikeOut,      // PDF 1.3
    Stamp,          // PDF 1.3
    Caret,          // PDF 1.5
    Ink,            // PDF 1.3
    Popup,          // PDF 1.3  // - supported
    FileAttachement,// PDF 1.3
    Sound,          // PDF 1.2
    Movie,          // PDF 1.2
    Widget,         // PDF 1.2  // - supported
    Screen,         // PDF 1.5
    PrinterMark,    // PDF 1.4
    TrapNet,        // PDF 1.3
    Watermark,      // PDF 1.6
    _3D,             // PDF 1.6
    RichMedia,      // PDF 1.7 ADBE ExtensionLevel 3 ALX: Petr P. Petrov
    WebMedia,       // PDF 1.7 IPDF ExtensionLevel 3
}

export enum NPDFAnnotationFlag {
    Invisible = 0x0001,
    Hidden = 0x0002,
    Print = 0x0004,
    NoZoom = 0x0008,
    NoRotate = 0x0010,
    NoView = 0x0020,
    ReadOnly = 0x0040,
    Locked = 0x0080,
    ToggleNoView = 0x0100,
    LockedContents = 0x0200
}

/**
 * Annotations are the core of all pdf widgets, this includes AcroForm fields, sticky notes, links, etc...
 * An Annotation is created via document page.createAnnotation. There is no other way to create a new annotation.
 * Annotations are a lower level PDF object, please familiarize yourself with the PDF Spec for usage.
 * {@link https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/PDF32000_2008.pdf}
 * @see IPage
 */
export interface IAnnotation {
    flags: NPDFAnnotationFlag
    title: string
    content: string
    destination: IDestination
    action: IAction
    open: boolean
    quadPoints: number[]
    color: NPDFColor
    attachment: IFileSpec

    setBorderStyle(v: NPDFAnnotationBorderStyle): void
    hasAppearanceStream(): boolean
    setAppearanceStream(xobj:IXObj): void
    getType(): NPDFAnnotationType
}

export type NPDFAnnotationBorderStyle = { horizontal: number, vertical: number, width: number }
