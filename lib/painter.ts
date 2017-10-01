import {IPage} from './page'
import {IImage} from './image'
const mod = require('bindings')('npdf')

export interface IPainter {
    setPage(page: IPage): void

    finishPage(): void

    drawText(): void

    drawImage(imgFile: string | IImage, x: number, y: number, width: number, height: number): void

    getPrecision(): number
}
export class Painter implements IPainter {
    private _instance: any
    constructor(page?:IPage) {
        this._instance = new mod.Painter()
        if(page)
            this._instance.setPage(page)
    }
    setPage(page: IPage): void {
        this._instance.setPage(page)
    }

    finishPage(): void {
        this._instance.finishPage()
    }

    drawText(): void {
        throw new Error("Method not implemented.");
    }

    drawImage(imgFile: IImage | string, x: number, y: number, width: number, height: number): void {
        this._instance.drawImage(imgFile, x, y, width, height)
    }

    getPrecision(): number {
        return this._instance.getPrecision()
    }
}