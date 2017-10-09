import {IPage} from './page'
import {IImage} from './image'
import {__mod} from './document'
// const mod = require('bindings')('npdf')

export interface IPainter {
    _instance:any
    page:IPage

    finishPage(): void

    drawText(): void

    drawImage(img: IImage, x: number, y: number, scale?: {width: number, height: number}): void

    getPrecision(): number
}
export class Painter implements IPainter {
    _instance: any
    get page() { return this._instance.page }
    set page(value:IPage) {this._instance.page = value._instance }

    constructor(page?:IPage) {
        this._instance = new __mod.Painter()
        if(page)
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
     * @param {IImage} img - an instance of Image
     * @param {number} x - x coordinate (bottom left position of image)
     * @param {number} y - y coordinate (bottom position of image)
     * @param {{width:number, heigth:number}} scale - optional scaling
     */
    drawImage(img: IImage, x: number, y: number, scale?: {width: number, height: number}): void {
        scale ? this._instance.drawImage(img, x, y, scale.width, scale.height) : this._instance.drawImage(img, x, y)
    }

    getPrecision(): number {
        return this._instance.getPrecision()
    }
}