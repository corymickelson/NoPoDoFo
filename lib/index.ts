/**
 * @type {NPdf}
 */
const mod = require('bindings')('npdf')

interface IPage {
    getRotation(): number

    getNumFields(): number

    getField(index: number): IField

    setRotation(degree: number): void

    getBottom(): number

    setBottom(value: number): void

    getLeft(): number

    setLeft(value: number): void

    getWidth(): number

    setWidth(value: number): void

    getHeight(): number

    setHeight(value: number): void

    addImg(file: string, dx: number, dy: number, scaleX: number, scaleY: number): void

    getFields(): [{
        name: string, alternateName: string, mappingName: string, required: boolean, readOnly: boolean, value: string | number,
        maxLen?: number, multiLine?: boolean, caption?: string, type: string, selected?: string
    }]

    setFieldValue(fieldIndex: number, value: number | string): void

    setFieldAlternateName(fieldIndex: number, value: string): void

    setFieldMappingName(fieldIndex: number, value: string): void

    setFieldRequired(fieldIndex: number, value: boolean): void

    getFieldIndex(fieldName: string): number
}

interface IField {

}

interface IDocument {
    load(file: string): void

    getPageCount(): number

    getPage(pageN: number): Page

    mergeDocument(doc: string): void

    deletePage(pageIndex: number): void

    getVersion(): number

    isLinearized(): boolean

    setPassword(password: string): void

    write(file: string): void
}

class Document implements IDocument {
    load(file: string): void {
        throw new Error("Method not implemented.");
    }

    getPageCount(): number {
        throw new Error("Method not implemented.");
    }

    getPage(pageN: number): Page {
        throw new Error("Method not implemented.");
    }

    mergeDocument(doc: string): void {
        throw new Error("Method not implemented.");
    }

    deletePage(pageIndex: number): void {
        throw new Error("Method not implemented.");
    }

    getVersion(): number {
        throw new Error("Method not implemented.");
    }

    isLinearized(): boolean {
        throw new Error("Method not implemented.");
    }

    setPassword(password: string): void {
        throw new Error("Method not implemented.");
    }

    write(file: string): void {
        throw new Error("Method not implemented.");
    }

    private _instance: NPdf.Document

    constructor(file: string) {
        this._instance = new mod.Document()
    }
}

class Page implements IPage {
    getRotation(): number {

        throw new Error("Method not implemented.");
    }

    getNumFields(): number {
        throw new Error("Method not implemented.");
    }

    getField(index: number): IField {
        throw new Error("Method not implemented.");
    }

    setRotation(degree: number): void {
        throw new Error("Method not implemented.");
    }

    getBottom(): number {
        throw new Error("Method not implemented.");
    }

    setBottom(value: number): void {
        throw new Error("Method not implemented.");
    }

    getLeft(): number {
        throw new Error("Method not implemented.");
    }

    setLeft(value: number): void {
        throw new Error("Method not implemented.");
    }

    getWidth(): number {
        throw new Error("Method not implemented.");
    }

    setWidth(value: number): void {
        throw new Error("Method not implemented.");
    }

    getHeight(): number {
        throw new Error("Method not implemented.");
    }

    setHeight(value: number): void {
        throw new Error("Method not implemented.");
    }

    addImg(file: string, dx: number, dy: number, scaleX: number, scaleY: number): void {
        throw new Error("Method not implemented.");
    }

    getFields(): [{ name: string; alternateName: string; mappingName: string; required: boolean; readOnly: boolean; value: (string | number); maxLen?: number; multiLine?: boolean; caption?: string; type: string; selected?: string }] {
        throw new Error("Method not implemented.");
    }

    setFieldValue(fieldIndex: number, value: number | string): void {
        throw new Error("Method not implemented.");
    }

    setFieldAlternateName(fieldIndex: number, value: string): void {
        throw new Error("Method not implemented.");
    }

    setFieldMappingName(fieldIndex: number, value: string): void {
        throw new Error("Method not implemented.");
    }

    setFieldRequired(fieldIndex: number, value: boolean): void {
        throw new Error("Method not implemented.");
    }

    getFieldIndex(fieldName: string): number {
        throw new Error("Method not implemented.");
    }

}

