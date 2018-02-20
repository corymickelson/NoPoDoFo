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
import {Image} from './image'
import {Stream} from './stream'
import {__mod, Document} from './document'
import {Rect} from "./rect";
import {NPDFPoint} from "./painter";
import { Obj } from './object';

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

export class Painter {
    private _instance: any

    get page() {
        return this._instance.page
    }

    set page(value: IPage) {
        this._instance.page = (value as any)._instance
    }

    get tabWidth(): number {
        return this._instance.tabWidth
    }

    set tabWidth(value:number) {
        this._instance.tabWidth = value
    }

    get canvas(): Stream {
        return this._instance.canvas
    }

    get font(): Font {
        const instance = this._instance.font
        return new Font(instance)
    }

    set font(font:Font) {
        this._instance.font = (font as any)._instance
    }

    get precision():number {
        return this._instance.precision
    }

    set precision(value:number) {
        this._instance.precision = value
    }

    constructor(doc:Document, page?: IPage) {
        this._instance = new __mod.Painter((doc as any)._instance)
        if (page)
            this._instance.page = (page as any)._instance
    }

    setColor(rgb:NPDFrgb):void {
        this._instance.setColor(rgb)
    }

    setStrokeWidth(w:number):void {
        this._instance.setStrokeWidth(w)
    }

    setGrey(v:number): void {
        if(v < 0.0 || v >1.0) {
            throw RangeError('grey value must be between 0 and 1')
        }
        this._instance.setGrey(v)
    }

    setStrokingGrey(v:number): void {
        if(v < 0.0 || v >1.0) {
            throw RangeError('grey value must be between 0 and 1')
        }
        this._instance.setStrokingGrey(v)
    }

    setColorCMYK(cmyk: NPDFcmyk):void {
        this._instance.setColorCMYK(cmyk)
    }

    setStrokingColorCMYK(cmyk:NPDFcmyk): void {
        this._instance.setStrokingColorCMYK(cmyk)
    }

    setStrokeStyle(style: NPDFStokeStyle): void {
        this._instance.setStrokeStyle(style)
    }

    setLineCapStyle(style: NPDFLineCapStyle): void {
        this._instance.setLineCapStyle(style)
    }

    setLineJoinStyle(style:NPDFLineJoinStyle): void {
        this._instance.setLineJoinStyle(style)
    }

    setClipRect(rect:Rect): void {
        if(rect instanceof Rect) {
            this._instance.setClipRect((rect as any)._instance)
        }
        else throw TypeError('clip rect requires argument type Rect')
    }

    setMiterLimit(v:number): void {
        this._instance.setMiterLimit(v)
    }

    rectangle(rect:Rect): void {
         if(rect instanceof Rect) {
            this._instance.rectangle((rect as any)._instance)
        }
        else throw TypeError('rectangle requires argument type Rect')
    }

    ellipse(points:NPDFPoint & {width:number, height:number}): void {
        this._instance.ellipse(points)
    }

    circle(points:NPDFPoint & {radius: number}): void {
        this._instance.circle(points)
    }

    closePath(): void {
        this._instance.closePath()
    }

    lineTo(point:NPDFPoint): void {
        this._instance.lineTo(point)
    }

    moveTo(point:NPDFPoint): void {
        this._instance.moveTo(point)
    }

    cubicBezierTo(p1:NPDFPoint, p2:NPDFPoint, p3:NPDFPoint): void {
        this._instance.cubicBezierTo(p1, p2, p3)
    }

    horizontalLineTo(v:number): void {
        this._instance.horizontalLineTo(v)
    }

    verticalLineTo(v:number): void {
        this._instance.verticalLineTo(v)
    }
    smoothCurveTo(p1:NPDFPoint, p2:NPDFPoint): void {
        this._instance.smoothCurveTo(p1, p2)
    }

    quadCurveTo(p1:NPDFPoint, p2:NPDFPoint): void {
        this._instance.quadCurveTo(p1, p2)
    }

    arcTo(p1:NPDFPoint, p2:NPDFPoint, rotation:number, large:boolean = false, sweep:boolean = false): void {
        this._instance.arcTo(p1, p2, rotation, large, sweep)
    }

    close(): void {
        this._instance.close()
    }

    stroke(): void {
        this._instance.stroke()
    }

    fill(): void {
        this._instance.fill()
    }

    strokeAndFill(): void {
        this._instance.strokeAndFill()
    }

    endPath(): void {
        this._instance.endPath()
    }

    clip(): void {
        this._instance.clip()
    }

    save():void {
        this._instance.save()
    }

    restore(): void {
        this._instance.restore()
    }

    setExtGState(state:ExtGState): void {
        this._instance.setExtGState(state)
    }

    getCurrentPath(): string {
        return this._instance.getCurrentPath()
    }

    drawLine(p1:NPDFPoint, p2:NPDFPoint): void {
        this._instance.drawLine(p1, p2)
    }

    drawText(point:NPDFPoint, text:string): void {
        this._instance.drawText(point, text)
    }

    drawTextAligned(point:NPDFPoint, text:string, alignment:NPDFAlignment): void {
        this._instance.drawTextAligned(point, text, alignment)
    }

    getMultiLineText(width:number, text:string, skipSpaces:boolean = false):Array<string> {
        return this._instance.getMultiLineText(width, text, skipSpaces)
    }

    bt(point:NPDFPoint): void {
        this._instance.beginText(point)
    }

    et(): void {
        this._instance.endText()
    }

    addText(text:string): void {
        this._instance.addText(text)
    }

    moveTextPosition(point:NPDFPoint): void {
        this._instance.moveTextPosition(point)
    }

    drawGlyph(point:NPDFPoint, glyph:string): void {
        this._instance.drawGlyph(point, glyph)
    }

    finishPage(): void {
        this._instance.finishPage()
    }

    /**
     *
     * @param {Image} img - an instance of Image
     * @param {number} x - x coordinate (bottom left position of image)
     * @param {number} y - y coordinate (bottom position of image)
     * @param {{width:number, heigth:number}} scale - optional scaling
     */
    drawImage(img: Image, x: number, y: number, scale?: { width: number, height: number }): void {
        scale ?
            this._instance.drawImage((img as any)._instance, x, y, scale.width, scale.height) :
            this._instance.drawImage((img as any)._instance, x, y)
    }

}

export interface FontMetrics {
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

export class Font {
    public get size() {
        return this._instance.size
    }

    public set size(value: number) {
        this._instance.size = value
    }

    public get scale() {
        return this._instance.scale
    }

    public set scale(value: number) {
        this._instance.scale = value
    }

    public get charSpace() {
        return this._instance.charSpace
    }

    public set(value: number) {
        this._instance.charSpace = value
    }

    public get wordSpace() {
        return this._instance.wordSpace
    }

    public set wordSpace(value: number) {
        this._instance.wordSpace = value
    }

    public get underline() {
        return this._instance.underline
    }

    public set underline(value: boolean) {
        this._instance.underline = value
    }

    public get strikeOut() {
        return this._instance.strikeOut
    }

    public set strikeOut(value: boolean) {
        this._instance.strikeOut = value
    }

    constructor(private _instance: any) {
    }

    isBold(): boolean {
        return this._instance.isBold()
    }

    isItalic(): boolean {
        return this._instance.isItalic()
    }

    getIdentifier(): string {
        return this._instance.getIdentifier()
    }

    getEncoding(): Encoding {
        const instance = this._instance.getEncoding()
        return new Encoding(instance)
    }

    getMetrics(): FontMetrics {
        return this._instance.getMetrics()
    }

    stringWidth(v:string): number {
        return this._instance.stringWidth(v)
    }

    write(content: string, stream: Stream): void {
        this._instance.write(content, (stream as any)._instance)
    }

    embed(): void {
        this._instance.embed()
    }
}

export class Encoding {
    constructor(private _instance: any) {
    }

    addToDictionary(target: {[key:string]: Obj}): void {
        this._instance.addToDictionary(target)
    }

    convertToUnicode(content: string, font: Font): string {
        return this._instance.convertToUnicode(content, (font as any)._instance)
    }

    convertToEncoding(content: string, font: Font): Buffer {
        return this._instance.convertToEncoding(content, (font as any)._instance)
    }
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
export class ExtGState {
    constructor(private _instance:any) {}

    setFillOpacity(v:number):void {
        this._instance.setFillOpacity(v)
    }
    setBlendMode(mode:NPDFBlendMode):void {
        this._instance.setBlendMode(mode)
    }
    setOverprint(v: boolean):void {
        this._instance.setOverprint(v)
    }
    setFillOverprint(v:boolean):void {
        this._instance.setFillOverprint(v)
    }
    setStrokeOpacity(v:number): void {
        this._instance.setStrokeOpacity(v)
    }
    setStrokeOverprint(v:boolean):void {
        this._instance.setStrokeOverprint(v)
    }
    setNonZeroOverprint(v:boolean):void {
        this._instance.setNonZeroOverprint(v)
    }
    setRenderingIntent(intent:NPDFRenderingIntent): void {
        this._instance.setRenderingIntent(intent)
    }
    setFrequency(v:number):void {
        this._instance.setFrequency(v)
    }

}
