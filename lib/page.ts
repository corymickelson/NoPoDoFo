import { IFieldInfo, Field } from './field'
import {  Rect } from './rect';
import {  Obj } from './object';
import { Annotation, NPdfAnnotationType, NPdfAnnotation } from './annotation';

export interface IPage {
    rotation: number
    trimBox: Rect
    number: number
    width: number
    height: number
    _instance: any

    getNumFields(): number
    getFieldsInfo(): Array<IFieldInfo>
    getField(index: number): Field
    getFields(): Array<Field>
    getFieldIndex(fieldName: string): number
    getContents(append: boolean): Obj
    getResources(): Obj
    getMediaBox(): Rect
    getBleedBox(): Rect
    getArtBox(): Rect
    getNumAnnots(): number
    createAnnotation(type: NPdfAnnotation, rect: Rect): Annotation
    getAnnotation(index: number): Annotation
    getAnnotations(): Array<Annotation>
    deleteAnnotation(index: number): void
}

export class Page implements IPage {
    get trimBox() {
        const trimBoxRect = this._instance.trimBox
        return new Rect([trimBoxRect.left, trimBoxRect.bottom, trimBoxRect.width, trimBoxRect.height])
    }
    set trimBox(rect: Rect) {
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

    get rotation(): number {
        return this._instance.rotation
    }

    set rotation(degree: number) {
        if (degree < 0 || degree > 270) {
            throw Error('rotation must be one of: 0, 90, 180, 270')
        }
        this._instance.rotation = degree
    }

    constructor(public _instance: any) { }

    getContents(append: boolean): Obj {
        const objInstance = this._instance.getContents(append)
        return new Obj(objInstance)
    }
    getResources(): Obj {
        const objInstance = this._instance.getResources()
        return new Obj(objInstance)
    }
    getMediaBox(): Rect {
        const mediaBoxPositions = this._instance.getMediaBox()
        const mediaBoxRect = new Rect([mediaBoxPositions.left, mediaBoxPositions.bottom, mediaBoxPositions.width, mediaBoxPositions.height])
        return mediaBoxPositions
    }
    getBleedBox(): Rect {
        const positions = this._instance.getBleedBox()
        return new Rect([positions.left, positions.bottom, positions.width, positions.height])
    }
    getArtBox(): Rect {
        const positions = this._instance.getArtBox()
        return new Rect([positions.left, positions.bottom, positions.width, positions.height])
    }
    getNumFields(): number {
        return this._instance.getNumFields()
    }
    getFieldsInfo(): IFieldInfo[] {
        return this._instance.getFieldsInfo()
    }
    getFieldIndex(fieldName: string): number {
        return this._instance.getFieldIndex(fieldName)
    }
    getField(index: number): Field {
        return new Field(this, index)
    }
    getFields(): Array<Field> {
        const fields = []
        let count = this.getNumFields(),
            i = 0;
        for (; i < count; i++) {
            fields[i] = new Field(this, i)
        }
        return fields
    }
    createAnnotation(type: NPdfAnnotation, rect: Rect):Annotation {
        const instance = this._instance.createAnnotation((type as number), rect._instance)
        return new Annotation(instance)
    }
    getAnnotation(index: number): Annotation {
        const instance = this._instance.getAnnotation(index)
        return new Annotation(instance)
    }
    getNumAnnots(): number {
        return this._instance.getNumAnnots()
    }

    getAnnotations(): Array<Annotation> {
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

    private static assertRect(rect: Rect) {
        if (rect.bottom === null ||
            rect.height === null ||
            rect.left === null ||
            rect.width === null) {
            throw Error("Rect must be initialized before use.")
        }
    }
}