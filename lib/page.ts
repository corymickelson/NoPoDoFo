import { IFieldInfo } from './field'
import { IRect, Rect } from './rect';
import { IPDObject, PDObject as NObject } from './object';
import {IAnnotation, Annotation, NPdfAnnotationType} from './annotation';

export interface IPage {
    rotation: number
    trimBox: IRect
    number: number
    width: number
    height: number

    getNumFields(): number
    getFields(): Array<IFieldInfo>
    getFieldIndex(fieldName: string): number
    getContents(append: boolean): IPDObject
    getResources(): IPDObject
    getMediaBox(): IRect
    getBleedBox(): IRect
    getArtBox(): IRect
    getNumAnnots(): number
    getAnnotation(index: number): IAnnotation
    getAnnotations(): Array<IAnnotation>
    deleteAnnotation(index: number): void
}

export class Page implements IPage {
    get trimBox() {
        const trimBoxRect = this._instance.trimBox
        return new Rect([trimBoxRect.left, trimBoxRect.bottom, trimBoxRect.width, trimBoxRect.height])
    }
    set trimBox(rect: IRect) {
        Page.assertRect(rect)
        this._instance.trimBox = rect
    }

    get number() {
        return this._instance.number
    }
    set number(n: number) {
        throw Error("Can not change page number. Use Document.deletePage() to adjust page order.")
    }

    get width() {
        return this._instance.width
    }
    set width(value: number) {
        if (Number.isNaN(value)) { }
        this._instance.width = value
    }

    get height() {
        return this._instance.height
    }
    set height(value: number) {
        if (Number.isNaN(value)) { }
        this._instance.height = value
    }

    get rotation():number {
        return this._instance.rotation
    }

    set rotation(degree:number) {
        if(degree < 0 || degree > 270) {
            throw Error('rotation must be one of: 0, 90, 180, 270')
        }
        this._instance.rotation = degree
    }

    constructor(private _instance: any) {}

    getContents(append: boolean): IPDObject {
        const objInstance = this._instance.getContents(append)
        return new NObject(objInstance)
    }
    getResources(): IPDObject {
        const objInstance = this._instance.getResources()
        return new NObject(objInstance)
    }
    getMediaBox(): IRect {
        const mediaBoxPositions = this._instance.getMediaBox()
        const mediaBoxRect = new Rect([mediaBoxPositions.left, mediaBoxPositions.bottom, mediaBoxPositions.width, mediaBoxPositions.height])
        return mediaBoxPositions
    }
    getBleedBox(): IRect {
        const positions = this._instance.getBleedBox()
        return new Rect([positions.left, positions.bottom, positions.width, positions.height])
    }
    getArtBox(): IRect {
        const positions = this._instance.getArtBox()
        return new Rect([positions.left, positions.bottom, positions.width, positions.height])
    }
    getNumFields(): number {
        return this._instance.getNumFields()
    }
    getFields(): IFieldInfo[] {
        return this._instance.getFields()
    }
    getFieldIndex(fieldName: string): number {
        return this._instance.getFieldIndex(fieldName)
    }
    createAnnotation(type: NPdfAnnotationType, rect: IRect): IAnnotation {
        const instance = this._instance.createAnnotation(type, rect)
        return new Annotation(instance)
    }
    getAnnotation(index: number): IAnnotation {
       const instance = this._instance.getAnnotation(index)
       return new Annotation(instance)
    }
    getNumAnnots(): number {
        return this._instance.getNumAnnots()
    }

    getAnnotations(): Array<IAnnotation> {
        const count = this.getNumAnnots()
        const output = []
        for (let i = 0; i < count; i++) {
            try {
                let item = this.getAnnotation(i)
                output.push(item)
            } catch (e) {
                throw e
            }
        }
        return output
    }

    deleteAnnotation(index: number): void {
        this._instance.deleteAnnotation(index)
    }

    private static assertRect(rect: IRect) {
        if (rect.bottom === null ||
            rect.height === null ||
            rect.left === null ||
            rect.width === null) {
            throw Error("Rect must be initialized before use.")
        }
    }
}