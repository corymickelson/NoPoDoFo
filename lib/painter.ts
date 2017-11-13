import {IPage} from './page'
import {Image} from './image'
import {Stream} from './stream'
import {__mod} from './document'
import {Dictionary} from "./dictionary";


export enum StokeStyle {
    Solid,
    Dash,
    Dot,
    DashDot,
    DashDotDot
}

export enum FontType {
    TrueType,
    Type1Pfa,
    Type1Pfb,
    Type1Base14,
    Type3
}

export enum ColorSpace {
    DeviceGray,
    DeviceRGB,
    DeviceCMYK,
    Separation,
    CieLab,
    Indexed
}

export enum TextRenderingMode {
    Fill,
    Stroke,
    FillAndStroke,
    Invisible,
    FillToClipPath,
    StrokeToClipPath,
    FillAndStrokeToClipPath,
    ToClipPath
}
export enum LineCapStyle {
    Butt,
    Round,
    Square
}

export enum LineJoinStyle {
    Miter,
    Round,
    Bevel
}
export enum VerticalAlignment {
    Top,
    Center,
    Bottom
}
export enum Alignment {
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
        this._instance.page = value._instance
    }

    constructor(page?: IPage) {
        this._instance = new __mod.Painter()
        if (page)
            this._instance.setPage(page)
    }

    finishPage(): void {
        this._instance.finishPage()
    }

    drawText(): void {
        throw new Error("Method not implemented.");
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
            this._instance.drawImage(img._instance, x, y, scale.width, scale.height) :
            this._instance.drawImage(img._instance, x, y)
    }

    getPrecision(): number {
        return this._instance.getPrecision()
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
    fontType: string
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

    addToDictionary(target: Dictionary): void {
        this._instance.addToDictionary(target)
    }

    convertToUnicode(content: string, font: Font): string {
        return this._instance.convertToUnicode(content, (font as any)._instance)
    }

    convertToEncoding(content: string, font: Font): Buffer {
        return this._instance.convertToEncoding(content, (font as any)._instance)
    }
}
