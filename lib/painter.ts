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
import {IPage} from './page'
import {IImage} from './image'
import {Stream} from './stream'
import {IBase} from './base-document'
import {IRect} from "./rect";
import {NPDFPoint} from "./painter";
import { IObj } from './object';

export type NPDFcmyk = [number, number, number, number]
export type NPDFrgb = [number, number, number]
export type NPDFGrayScale = number
export type NPDFColor = NPDFrgb | NPDFcmyk | NPDFGrayScale
export type NPDFPoint = {x:number, y:number}

export enum NPDFStokeStyle {
    Solid,
    Dash,
    Dot,
    DashDot,
    DashDotDot
}

export enum NPDFHighlightingMode {
    None,           ///< Do no highlighting
    Invert,         ///< Invert the PdfField
    InvertOutline,  ///< Invert the fields border
    Push,           ///< Display the fields down appearance (requires an additional appearance stream to be set)
    Unknown = 0xff
}
export enum NPDFFontType {
    TrueType,
    Type1Pfa,
    Type1Pfb,
    Type1Base14,
    Type3
}

export enum NPDFColorSpace {
    DeviceGray,
    DeviceRGB,
    DeviceCMYK,
    Separation,
    CieLab,
    Indexed
}

export enum NPDFTextRenderingMode {
    Fill,
    Stroke,
    FillAndStroke,
    Invisible,
    FillToClipPath,
    StrokeToClipPath,
    FillAndStrokeToClipPath,
    ToClipPath
}
export enum NPDFLineCapStyle {
    Butt,
    Round,
    Square
}

export enum NPDFLineJoinStyle {
    Miter,
    Round,
    Bevel
}
export enum NPDFVerticalAlignment {
    Top,
    Center,
    Bottom
}
export enum NPDFAlignment {
    Left,
    Center,
    Bottom
}

export interface IPainter {
    tabWidth: number
    readonly canvas: Stream
    font?: IFont
    precision: number
    new(doc:IBase):IPainter
    setPage(page: IPage): void
    setColor(rgb:NPDFrgb):void
    setStrokeWidth(w:number):void
    setGrey(v:number): void
    setStrokingGrey(v:number): void
    setColorCMYK(cmyk: NPDFcmyk):void
    setStrokingColorCMYK(cmyk:NPDFcmyk): void
    setStrokeStyle(style: NPDFStokeStyle): void
    setLineCapStyle(style: NPDFLineCapStyle): void
    setLineJoinStyle(style:NPDFLineJoinStyle): void
    setClipRect(rect:IRect): void
    setMiterLimit(v:number): void
    rectangle(rect:IRect): void
    ellipse(points:NPDFPoint & {width:number, height:number}): void
    circle(points:NPDFPoint & {radius: number}): void
    closePath(): void
    lineTo(point:NPDFPoint): void
    moveTo(point:NPDFPoint): void
    cubicBezierTo(p1:NPDFPoint, p2:NPDFPoint, p3:NPDFPoint): void
    horizontalLineTo(v:number): void
    verticalLineTo(v:number): void
    smoothCurveTo(p1:NPDFPoint, p2:NPDFPoint): void
    quadCurveTo(p1:NPDFPoint, p2:NPDFPoint): void
    arcTo(p1:NPDFPoint, p2:NPDFPoint, rotation:number, large?:boolean, sweep?:boolean): void
    close(): void
    stroke(): void
    fill(): void
    strokeAndFill(): void
    endPath(): void
    clip(): void
    save():void
    restore(): void
    setExtGState(state:IExtGState): void
    getCurrentPath(): string
    drawLine(p1:NPDFPoint, p2:NPDFPoint): void
    drawText(point:NPDFPoint, text:string): void
    drawTextAligned(point:NPDFPoint, text:string, alignment:NPDFAlignment): void
    drawMultiLineText(rect:IRect, value:string, alignment?:NPDFAlignment, vertical?:NPDFVerticalAlignment): void
    getMultiLineText(width:number, text:string, skipSpaces?:boolean):Array<string>
    bt(point:NPDFPoint): void
    et(): void
    addText(text:string): void
    moveTextPosition(point:NPDFPoint): void
    drawGlyph(point:NPDFPoint, glyph:string): void
    finishPage(): void
    /**
     *
     * @param {Image} img - an instance of Image
     * @param {number} x - x coordinate (bottom left position of image)
     * @param {number} y - y coordinate (bottom position of image)
     * @param {{width:number, heigth:number}} scale - optional scaling
     */
    drawImage(img: IImage, x: number, y: number, scale?: { width: number, height: number }): void

}

export interface NPDFFontMetrics {
    lineSpacing: number
    underlineThickness: number
    underlinePosition: number
    strikeOutThickness: number
    strikeOutPosition: number
    fileName: string
    fontData: string
    fontWeight: number
    fontSize: number
    fontScale: number
    charSpace: number
    wordSpace: number
    fontType?: string
}

export interface IFont {
    object: IObj
    size: number
    scale: number
    charSpace: number
    wordSpace: number
    underline: boolean
    strikeOut: boolean
    identifier: string
    isBold(): boolean
    isItalic(): boolean
    getEncoding(): IEncoding
    getMetrics(): NPDFFontMetrics
    stringWidth(v:string): number
    write(content: string, stream: Stream): void
    embed(): void
}
export interface IEncoding {
    new(): IEncoding
    addToDictionary(target:IObj): void
    convertToUnicode(content:string, font: IFont): string
    convertToEncoding(content: string, font: IFont): Buffer
}

export enum NPDFBlendMode {
    Normal = "Normal",
    Multiply = "Multiply",
    Screen = "Screen",
    Overlay = "Overlay",
    Darken = "Darken",
    Lighten = "Lighten",
    ColorDodge = "ColorDodge",
    ColorBurn = "ColorBurn",
    HardLight = "HardLight",
    SoftLight = "SoftLight",
    Difference = "Difference",
    Exclusion = "Exclusion",
    Hue = "Hue",
    Saturation = "Saturation",
    Color = "Color",
    Luminosity = "Luminosity"
}
export enum NPDFRenderingIntent {
    AbsoluteColorimetric = "AbsoluteColorimetric",
    RelativeColorimetric = "RelativeColorimetric",
    Perceptual = "Perceptual",
    Saturation = "Saturation"
}
export interface IExtGState {
    new(): IExtGState
    setFillOpacity(v:number):void
    setBlendMode(mode:NPDFBlendMode):void
    setOverprint(v: boolean):void
    setFillOverprint(v:boolean):void
    setStrokeOpacity(v:number): void
    setStrokeOverprint(v:boolean):void
    setNonZeroOverprint(v:boolean):void
    setRenderingIntent(intent:NPDFRenderingIntent): void
    setFrequency(v:number):void

}
