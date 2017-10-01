const mod = require('../src/npdf')
import { IPage, Page } from './page'
import { Document, IDocument } from './document'

export interface IRect {
    left:number
    bottom:number
    width:number
    height:number

}
export class Rect implements IRect {
    private _instance: any

    constructor(position?: IPage | Array<number>) {
        if (Array.isArray(position)) {

        }
        if (position !== null) {

            this._instance = new mod.Rect(
                (position as number[])[0],
                (position as number[])[1],
                (position as number[])[2],
                (position as number[])[3])
        }
        else this._instance = new mod.Rect()
    }
    get bottom(): number {
        return this._instance.bottom
    }

    set bottom(value: number) {
        this._instance.bottom = value
    }

    get left(): number {
        return this._instance.left
    }

    set left(value: number) {
        this._instance.left = value
    }

    get width(): number {
        return this._instance.width
    }

    set width(value: number) {
        this._instance.width = value
    }

    get height(): number {
        return this._instance.height
    }

    set height(value: number) {
        this._instance.height = value
    }

    intersect(rect: IRect): void {
        this._instance.intersect(rect)
    }
}