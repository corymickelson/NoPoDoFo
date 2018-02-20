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
import { Page } from './page'
import { Rect } from './rect'
import { NPDFrgb } from './painter';

export interface IAnnotation {
    title: string
    flag: number
    color: NPDFrgb
    hasAppearanceStream(): boolean
    setBorderStyle(horizontalRadius: number, verticalRadius: number, width: number): void
    hasDestination(): boolean
    hasAction(): boolean
    getType(): NPDFAnnotationType
    setFileAttachment(): void
    hasFileAttachment(): boolean
}
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

export enum NPDFAction {
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
}
/**
 * @desc Red, Green, Blue
 */
export class Annotation {
    get quadPoints() {
        throw Error("unimplemented")
    }
    set quadPoints(value:Array<number>) {
        throw Error("unimplemented")
    }
    get title() {
        return this._instance.title
    }
    set title(value:string) {
        this._instance.title = value
    }
    get flag(): NPDFAnnotationFlag {
        return this._instance.flag
    }
    set flag(value: NPDFAnnotationFlag) {
        this._instance.flag = value
    }
    get color() {
        return this._instance.color
    }
    set color(value: NPDFrgb) {
        let rgbErr = Error("RGB value must be an integer >= 0 || <= 256")
        if (value.length !== 3) {
            throw rgbErr
        }
        value.forEach(e => {
            if (Number.isInteger(e) === false) {
                throw rgbErr
            }
            if (Number.isNaN(e)) {
                throw rgbErr
            }
        });
        this._instance.color = value
    }

    constructor(private _instance:any) {}

    hasAppearanceStream(): boolean {
        return this._instance.hasAppearanceStream()
    }
    setBorderStyle(horizontalRadius: number, verticalRadius: number, width: number): void {
        return this._instance.setBorderStyle(horizontalRadius, verticalRadius, width)
    }
    hasDestination(): boolean {
        return this._instance.hasDestination()
    }
    hasAction(): boolean {
        return this._instance.hasAction()
    }
    getType(): NPDFAnnotationType {
        return this._instance.getType()
    }
    setFileAttachment(): void {
        throw new Error("Method not implemented.");
    }
    hasFileAttachment(): boolean {
        return this._instance.hasFileAttachment()
    }
}