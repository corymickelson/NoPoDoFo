const mod = require('../src/npdf')

export interface IPage {
    getRotation(): number

    getNumFields(): number

    setRotation(degree: number): void

    getFields(): [{
        name: string, alternateName: string, mappingName: string, required: boolean, readOnly: boolean, value: string | number,
        maxLen?: number, multiLine?: boolean, caption?: string, type: string, selected?: string
    }]

    getFieldIndex(fieldName: string): number
}

export interface IField {
    getType(): string

    getFieldName(): string

    getAlternateName(): string

    getMappingName(): string

    isRequired(): boolean

    setRequired(required: boolean): void

    setAlternateName(name: string): void

    setMappingName(name: string): void
}

export interface IPainter {
    setPage(page: IPage): void

    finishPage(): void

    drawText(): void

    drawImage(imgFile: string, x:number, y:number, width:number, height:number): void

    getPrecision(): number
}

export interface ITextField {
    getText(): string
    setText(value: string): void
}

export interface IDocument {

    load(file: string): void

    getPageCount(): number

    getPage(pageN: number): IPage

    mergeDocument(doc: string): void

    deletePage(pageIndex: number): void

    getVersion(): number

    isLinearized(): boolean

    setPassword(password: string): void

    write(file: string): void
}

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

export interface IImage {

}

export interface IAnnotation {

}

export interface ISignature {

}

export class Document implements IDocument {

    private _instance:any

    constructor(file: string) {
        this._instance = new mod.Document()
    }

    load(file: string): void {
        this._instance.load(file)
    }

    getPageCount(): number {
        return this._instance.getPageCount()
    }

    getPage(pageN: number): IPage {
        const page:IPage = this._instance.getPage(pageN)
        return page;
    }

    mergeDocument(doc: string): void {
        this._instance.mergeDocument(doc)
    }

    deletePage(pageIndex: number): void {
        this._instance.deletePage(pageIndex)
    }

    getVersion(): number {
        return this._instance.getVersion()
    }

    isLinearized(): boolean {
        return this._instance.isLinearized()
    }

    setPassword(password: string): void {
        this._instance.setPassword(password)
    }

    write(file: string): void {
        this._instance.write(file)
    }

}


export class Field implements IField {
    private _instance:any
    constructor(page:IPage, fieldIndex:number) {
        this._instance = new mod.Field(page, fieldIndex)
    }

    getType(): string {
        return this._instance.getType()
    }

    getFieldName(): string {
        return this._instance.getFieldName()
    }

    getAlternateName(): string {
        return this._instance.getAlternateName()
    }

    getMappingName(): string {
        return this._instance.getMappingName()
    }

    isRequired(): boolean {
        return this._instance.isRequired()
    }

    setRequired(required: boolean): void {
        this._instance.setRequired(required)
    }

    setAlternateName(name: string): void {
        this._instance.setAlternateName(name)
    }

    setMappingName(name: string): void {
        this._instance.setMappingName(name)
    }
}

export class TextField implements ITextField {
    private _instance:any
    constructor(field:IField) {
        this._instance = new mod.TextField(field)
    }
    getText(): string {
        return this._instance.getText()
    }

    setText(value: string): void {
        return this._instance.setText(value)
    }
}

export class Painter implements IPainter {
    private _instance: any
    constructor() {
        this._instance = new mod.Painter()
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

    drawImage(imgFile: string, x:number, y:number, width:number, height:number): void {
        this._instance.drawImage(imgFile, x,y,width,height)
    }

    getPrecision(): number {
        return this._instance.getPrecision()
    }
}

export class Rect implements IRect {
    private _instance:any
    constructor(page:IPage) {
        this._instance = new mod.Rect(page)
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

export class Image implements IImage {}

export class Signature implements ISignature {}

export class Annotation implements IAnnotation {}
