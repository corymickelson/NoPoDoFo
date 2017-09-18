const mod = require('../src/npdf')

export interface IFieldInfo {
    name: string,
    alternateName: string,
    mappingName: string,
    required: boolean,
    readOnly: boolean,
    value: string | number,
    maxLen?: number,
    multiLine?: boolean,
    caption?: string,
    type: string,
    selected?: string
}
export interface IPage {
    getRotation(): number

    getNumFields(): number

    /**
     * @description Set page rotate dict entry. Value must be an increment of 90, where 0 is vertical.
     */
    setRotation(degree: number): void

    /**
     * @description Get information on all fields on the page
     */
    getFields(): Array<IFieldInfo>

    /**
     * @description Get field index of fieldname.
     */
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

    drawImage(imgFile: string | IImage, x: number, y: number, width: number, height: number): void

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
    getWidth(): number
    getHeight(): number
    setFile(file: string): void
    setData(data: Buffer): void
    isLoaded(): boolean
}
export interface IAnnotation {

}
export interface ISignature {

}
export enum NPdfAnnotation {
    Text = 0,                   // - supported
    Link,                       // - supported
    FreeText,       // PDF 1.3  // - supported
    Line,           // PDF 1.3  // - supported
    Square,         // PDF 1.3
    Circle,         // PDF 1.3
    Polygon,        // PDF 1.5
    PolyLine,       // PDF 1.5
    Highlight,      // PDF 1.3
    Underline,      // PDF 1.3
    Squiggly,       // PDF 1.4
    StrikeOut,      // PDF 1.3
    Stamp,          // PDF 1.3
    Caret,          // PDF 1.5
    Ink,            // PDF 1.3
    Popup,          // PDF 1.3  // - supported
    FileAttachement,// PDF 1.3
    Sound,          // PDF 1.2
    Movie,          // PDF 1.2
    Widget,         // PDF 1.2  // - supported
    Screen,         // PDF 1.5
    PrinterMark,    // PDF 1.4
    TrapNet,        // PDF 1.3
    Watermark,      // PDF 1.6
    _3D,             // PDF 1.6
    RichMedia,      // PDF 1.7 ADBE ExtensionLevel 3 ALX: Petr P. Petrov
    WebMedia,       // PDF 1.7 IPDF ExtensionLevel 3
}

export enum NpdfAnnotationFlag {
    Invisible    = 0x0001,
    Hidden       = 0x0002,
    Print        = 0x0004,
    NoZoom       = 0x0008,
    NoRotate     = 0x0010,
    NoView       = 0x0020,
    ReadOnly     = 0x0040,
    Locked       = 0x0080,
    ToggleNoView = 0x0100,
    LockedContents = 0x0200
}
 export enum NpdfAction {
    GoTo = 0,
    GoToR,
    GoToE,
    Launch,    
    Thread,
    URI,
    Sound,
    Movie,
    Hide,
    Named,
    SubmitForm,
    ResetForm,
    ImportData,
    JavaScript,
    SetOCGState,
    Rendition,
    Trans,
    GoTo3DView,
    RichMediaExecute,
 }
/**
 * @class Document
 * @description A Pdf document
 */
export class Document implements IDocument {

    private _instance: any
    private _loaded: boolean = false;
    private _pageCount:number

    get pageCount() {
        return this._pageCount
    }

    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @returns void
     */
    constructor(file: string) {
        this._instance = new mod.Document()
        if(file) {
            this._instance.load(file)
            this._pageCount = this._instance.getPageCount()            
            this._loaded = true
        }
    }

    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     */
    load(file: string): void {
        this._instance.load(file)
        this._loaded = true
    }

    getPageCount(): number {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getPageCount()
    }

    getPage(pageN: number): IPage {
        if(this.pageCount > pageN || this.pageCount < 0) {
            throw new RangeError("pageN out of range")
        }
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        const page: IPage = this._instance.getPage(pageN)
        return page;
    }

    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     */
    mergeDocument(doc: string): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        this._instance.mergeDocument(doc)
    }

    deletePage(pageIndex: number): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        if(this.pageCount < pageIndex || pageIndex < 0) {
            throw new RangeError('page index out of range')
        }
        this._instance.deletePage(pageIndex)
    }

    getVersion(): number {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.getVersion()
    }

    isLinearized(): boolean {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        return this._instance.isLinearized()
    }

    setPassword(password: string): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        this._instance.setPassword(password)
    }

    write(file: string): void {
        if(!this._loaded) {
            throw new Error('load a pdf file before calling this method')
        }
        this._instance.write(file)
    }

}


export type FieldType = 'TextField' | 'CheckBox' | 'ListBox' | 'RadioButton' | 'ComboBox' | 'PushButton' | 'Signature'
export class Field implements IField {
    private _instance: any
    constructor(page: IPage, fieldIndex: number) {
        this._instance = new mod.Field(page, fieldIndex)
    }

    getType(): FieldType {
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
    private _instance: any
    constructor(field: IField) {
        this._instance = new mod.TextField(field)
    }
    getText(): string {
        return this._instance.getText()
    }
    setText(value: string): void {
        return this._instance.setText(value)
    }
}

/**
 * @class Painter
 * @description Draw text / image to page
 */
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

export class Image implements IImage {
    private _instance: any
    constructor(private _doc: IDocument, data?: string | Buffer) {
        if (data) {
            this._instance = new mod.Image(_doc, data)
        }
        else {
            this._instance = new mod.Image(_doc)
        }
    }
    getWidth(): number {
        return this._instance.getWidth()
    }
    getHeight(): number {
        return this._instance.getHeight()
    }
    setFile(file: string): void {
        this._instance.setFile(file)
    }
    /**
     * @description Embeds data as an image on the document. To use this image pass to Painter.drawImage.
     *      New instances will reuse embedded img object if passed the same image reference.
     * @throws {TypeError} - throw type error on invalid arguments.
     * @param data img data
     * @returns void
     */
    setData(data: Buffer | string): void {
        if(Buffer.isBuffer(data)) this._instance.setData(data.toString('utf8'))
        else if(typeof data === 'string' || (data as any) instanceof String)
            this._instance.setData(data)
        else throw new TypeError("Image.setData requires a single argument of type string | Buffer")
    }
    isLoaded(): boolean {
        return this._instance.isLoaded()
    }

}

export class Signature implements ISignature { }

export class Annotation implements IAnnotation {
    constructor(page:IPage, type:NPdfAnnotation, rect:IRect) {}
 }
