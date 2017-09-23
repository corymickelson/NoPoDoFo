const mod = require('../src/npdf')
import {Document, IDocument, IPage} from './document'

export interface IRect {
    getBottom(): number

    setBottom(value: number): void

    getLeft(): number

    setLeft(value: number): void

    getWidth(): number

    setWidth(value: number): void

    getHeight(): number

    setHeight(value: number): void
}
export class Rect implements IRect {
    private _instance: any
    constructor(page?: IPage) {
        if(page) this._instance = new mod.Rect(page)
        else this._instance = new mod.Rect()
    }
    getBottom(): number {
        return this._instance.getBottom()
    }

    setBottom(value: number): void {
        this._instance.setBottom(value)
    }

    getLeft(): number {
        return this._instance.getLeft()
    }

    setLeft(value: number): void {
        this._instance.setLeft(value)
    }

    getWidth(): number {
        return this._instance.getWidth()
    }

    setWidth(value: number): void {
        this._instance.setWidth(value)
    }

    getHeight(): number {
        return this._instance.getHeight()
    }

    setHeight(value: number): void {
        this._instance.setHeight(value)
    }
}