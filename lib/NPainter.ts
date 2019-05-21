import {
    nopodofo,
    NPDFAlignment,
    NPDFLineCapStyle,
    NPDFLineJoinStyle,
    NPDFPoint,
    NPDFStokeStyle,
    NPDFVerticalAlignment
} from "../index";
import {NDocument} from "./NDocument";
import {NPage} from "./NPage";
import {NXObject} from "./NXObject";
import {NExtGState} from "./NExtGState";
import {NFont} from "./NFont";

export class NPainter {
    private self: nopodofo.Painter;

    get font(): NFont {
        if (!this.self.font) {
            throw Error('Font has not been set, please see the NoPoDoFo.Painter docs')
        } else {
            return new NFont(this.parent, this.self.font)
        }
    }

    set font(value: NFont) {
        this.self.font = (value as any).self;
    }

    get precision(): number {
        return this.self.precision;
    }

    set precision(value: number) {
        this.self.precision = value;
    }

    get tabWidth(): number {
        return this.self.tabWidth;
    }

    set tabWidth(value: number) {
        this.self.tabWidth = value;
    }

    get canvas(): nopodofo.Stream {
        return this.self.canvas
    }

    constructor(private parent: NDocument, self: nopodofo.Painter) {
        this.self = self
    }

    addText(text: string): void {
        this.self.addText(text)
    }

    arcTo(p1: NPDFPoint, p2: NPDFPoint, rotation: number, large?: boolean, sweep?: boolean): void {
        this.self.arcTo(p1, p2, rotation, large, sweep)
    }

    beginText(point: NPDFPoint): void {
        this.self.beginText(point)
    }

    circle(points: NPDFPoint & { radius: number }): void {
        this.self.circle(points)
    }

    clip(): void {
        this.self.clip()
    }

    close(): void {
        this.self.close()
    }

    closePath(): void {
        this.self.closePath()
    }

    cubicBezierTo(p1: NPDFPoint, p2: NPDFPoint, p3: NPDFPoint): void {
        this.self.cubicBezierTo(p1, p2, p3)
    }

    drawGlyph(point: NPDFPoint, glyph: string): void {
        this.self.drawGlyph(point, glyph)
    }

    drawImage(img: nopodofo.Image, x: number, y: number, opts?: { width?: number; height?: number; scaleX?: number; scaleY?: number }): void {
        this.self.drawImage(img, x, y, opts)
    }

    drawLine(p1: NPDFPoint, p2: NPDFPoint): void {
        this.self.drawLine(p1, p2)
    }

    drawMultiLineText(rect: nopodofo.Rect, value: string, alignment?: NPDFAlignment, vertical?: NPDFVerticalAlignment): void {
        this.self.drawMultiLineText(rect, value, alignment, vertical)
    }

    drawText(point: NPDFPoint, text: string): void {
        this.self.drawText(point, text)
    }

    drawTextAligned(point: NPDFPoint & { width: number }, text: string, alignment: NPDFAlignment): void {
        this.self.drawTextAligned(point, text, alignment)
    }

    ellipse(points: NPDFPoint & { width: number; height: number }): void {
        this.self.ellipse(points)
    }

    endPath(): void {
        this.self.endPath()
    }

    endText(): void {
        this.self.endText()
    }

    fill(): void {
        this.self.fill()
    }

    finishPage(): void {
        this.self.finishPage()
    }

    getCurrentPath(): string {
        return this.self.getCurrentPath()
    }

    getMultiLineText(width: number, text: string, skipSpaces?: boolean): Array<string> {
        return this.self.getMultiLineText(width, text, skipSpaces)
    }

    horizontalLineTo(v: number): void {
        this.self.horizontalLineTo(v)
    }

    lineTo(point: NPDFPoint): void {
        this.self.lineTo(point)
    }

    moveTextPosition(point: NPDFPoint): void {
        this.self.moveTextPosition(point)
    }

    moveTo(point: NPDFPoint): void {
        this.self.moveTo(point)
    }

    quadCurveTo(p1: NPDFPoint, p2: NPDFPoint): void {
        this.self.quadCurveTo(p1, p2)
    }

    rectangle(rect: nopodofo.Rect): void {
        this.self.rectangle(rect)
    }

    restore(): void {
        this.self.restore()
    }

    save(): void {
        this.self.save()
    }

    setClipRect(rect: nopodofo.Rect): void {
        this.self.setClipRect(rect)
    }

    setColor(rgb: nopodofo.Color): void {
        this.self.setColor(rgb)
    }

    setColorCMYK(cmyk: nopodofo.Color): void {
        this.self.setColorCMYK(cmyk)
    }

    setExtGState(state: NExtGState): void {
        this.self.setExtGState((state as any).self)
    }

    setGrey(v: number): void {
        this.self.setGrey(v)
    }

    setLineCapStyle(style: NPDFLineCapStyle): void {
        this.self.setLineCapStyle(style)
    }

    setLineJoinStyle(style: NPDFLineJoinStyle): void {
        this.self.setLineJoinStyle(style)
    }

    setMiterLimit(v: number): void {
        this.self.setMiterLimit(v)
    }

    setPage(page: NPage | NXObject): void {
        this.self.setPage(page.self)
    }

    setStrokeStyle(style: NPDFStokeStyle): void {
        this.self.setStrokeStyle(style)
    }

    setStrokeWidth(w: number): void {
        this.self.setStrokeWidth(w)
    }

    setStrokingColorCMYK(cmyk: nopodofo.Color): void {
        this.self.setStrokingColorCMYK(cmyk)
    }

    setStrokingGrey(v: number): void {
        this.self.setStrokingGrey(v)
    }

    smoothCurveTo(p1: NPDFPoint, p2: NPDFPoint): void {
        this.self.smoothCurveTo(p1, p2)
    }

    stroke(): void {
        this.self.stroke()
    }

    strokeAndFill(): void {
        this.self.strokeAndFill()
    }

    verticalLineTo(v: number): void {
        this.self.verticalLineTo(v)
    }

}
