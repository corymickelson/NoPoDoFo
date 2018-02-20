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
import { IFieldInfo, Field } from './field'
import {  Rect } from './rect';
import {  Obj } from './object';
import { Annotation, NPDFAnnotationType, NPDFAnnotation } from './annotation';

export interface IPage {
    rotation: number
    trimBox: Rect
    number: number
    width: number
    height: number

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
    createAnnotation(type: NPDFAnnotation, rect: Rect): Annotation
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

    constructor(private _instance: any) { }

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
        // return new Field(this, index)
        return new Field(this._instance.getField(index))
    }
    getFields(): Array<Field> {
        const fields = []
        const count = this.getNumFields();
        let i = 0;
        for (; i < count; i++) {
            fields[i] = ((new Field(this._instance.getField(i))) as any)
        }
        return fields as Array<Field>
    }
    createAnnotation(type: NPDFAnnotation, rect: Rect):Annotation {
        const instance = this._instance.createAnnotation((type as number), (rect as any)._instance)
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
        const output: Array<Annotation> = []
        for (let i = 0; i < count; i++) {
            try {
                const item = this.getAnnotation(i);
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