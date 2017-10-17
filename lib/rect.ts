import { Page } from './page'
import { Document, __mod } from './document'

export class Rect {
    _instance: any

    constructor(position?: Page | Array<number>) {
        if (Array.isArray(position)) {
            this._instance = new __mod.Rect(
                (position as number[])[0],
                (position as number[])[1],
                (position as number[])[2],
                (position as number[])[3])
        }
        else if(position instanceof Page) {
            this._instance = new __mod.Rect(position)
        }
        else {
            this._instance = new __mod.Rect()
        }
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

    intersect(rect: Rect): void {
        this._instance.intersect(rect)
    }
}