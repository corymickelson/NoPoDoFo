/// <reference types="node" />
export interface IFieldInfo {
    name: string;
    alternateName: string;
    mappingName: string;
    required: boolean;
    readOnly: boolean;
    value: string | number;
    maxLen?: number;
    multiLine?: boolean;
    caption?: string;
    type: string;
    selected?: string;
}
export interface IPage {
    getRotation(): number;
    getNumFields(): number;
    setRotation(degree: number): void;
    getFields(): Array<IFieldInfo>;
    getFieldIndex(fieldName: string): number;
}
export interface IField {
    getType(): string;
    getFieldName(): string;
    getAlternateName(): string;
    getMappingName(): string;
    isRequired(): boolean;
    setRequired(required: boolean): void;
    setAlternateName(name: string): void;
    setMappingName(name: string): void;
}
export interface IPainter {
    setPage(page: IPage): void;
    finishPage(): void;
    drawText(): void;
    drawImage(imgFile: string | IImage, x: number, y: number, width: number, height: number): void;
    getPrecision(): number;
}
export interface ITextField {
    getText(): string;
    setText(value: string): void;
}
export interface IDocument {
    load(file: string): void;
    getPageCount(): number;
    getPage(pageN: number): IPage;
    mergeDocument(doc: string): void;
    deletePage(pageIndex: number): void;
    getVersion(): number;
    isLinearized(): boolean;
    setPassword(password: string): void;
    write(file: string): void;
}
export interface IRect {
    getBottom(): number;
    setBottom(value: number): void;
    getLeft(): number;
    setLeft(value: number): void;
    getWidth(): number;
    setWidth(value: number): void;
    getHeight(): number;
    setHeight(value: number): void;
}
export interface IImage {
    getWidth(): number;
    getHeight(): number;
    setFile(file: string): void;
    setData(data: Buffer): void;
    isLoaded(): boolean;
}
export interface IAnnotation {
}
export interface ISignature {
}
export declare enum NPdfAnnotation {
    Text = 0,
    Link = 1,
    FreeText = 2,
    Line = 3,
    Square = 4,
    Circle = 5,
    Polygon = 6,
    PolyLine = 7,
    Highlight = 8,
    Underline = 9,
    Squiggly = 10,
    StrikeOut = 11,
    Stamp = 12,
    Caret = 13,
    Ink = 14,
    Popup = 15,
    FileAttachement = 16,
    Sound = 17,
    Movie = 18,
    Widget = 19,
    Screen = 20,
    PrinterMark = 21,
    TrapNet = 22,
    Watermark = 23,
    _3D = 24,
    RichMedia = 25,
    WebMedia = 26,
}
/**
 * @class Document
 * @description A Pdf document
 */
export declare class Document implements IDocument {
    private _instance;
    private _loaded;
    private _pageCount;
    readonly pageCount: number;
    /**
     * @constructor
     * @param {string} [file] - pdf file path (optional)
     * @returns void
     */
    constructor(file: string);
    /**
     * @description load pdf file
     * @param {string} file - pdf file path
     */
    load(file: string): void;
    getPageCount(): number;
    getPage(pageN: number): IPage;
    /**
     * @description Append doc to the end of the loaded doc
     * @param {string} doc - pdf file path
     */
    mergeDocument(doc: string): void;
    deletePage(pageIndex: number): void;
    getVersion(): number;
    isLinearized(): boolean;
    setPassword(password: string): void;
    write(file: string): void;
}
export declare type FieldType = 'TextField' | 'CheckBox' | 'ListBox' | 'RadioButton' | 'ComboBox' | 'PushButton' | 'Signature';
export declare class Field implements IField {
    private _instance;
    constructor(page: IPage, fieldIndex: number);
    getType(): FieldType;
    getFieldName(): string;
    getAlternateName(): string;
    getMappingName(): string;
    isRequired(): boolean;
    setRequired(required: boolean): void;
    setAlternateName(name: string): void;
    setMappingName(name: string): void;
}
export declare class TextField implements ITextField {
    private _instance;
    constructor(field: IField);
    getText(): string;
    setText(value: string): void;
}
/**
 * @class Painter
 * @description Draw text / image to page
 */
export declare class Painter implements IPainter {
    private _instance;
    constructor(page?: IPage);
    setPage(page: IPage): void;
    finishPage(): void;
    drawText(): void;
    drawImage(imgFile: IImage | string, x: number, y: number, width: number, height: number): void;
    getPrecision(): number;
}
export declare class Rect implements IRect {
    private _instance;
    constructor(page?: IPage);
    getBottom(): number;
    setBottom(value: number): void;
    getLeft(): number;
    setLeft(value: number): void;
    getWidth(): number;
    setWidth(value: number): void;
    getHeight(): number;
    setHeight(value: number): void;
}
export declare class Image implements IImage {
    private _doc;
    private _instance;
    constructor(_doc: IDocument, data?: string | Buffer);
    getWidth(): number;
    getHeight(): number;
    setFile(file: string): void;
    /**
     * @description Embeds data as an image on the document. To use this image pass to Painter.drawImage.
     *      New instances will reuse embedded img object if passed the same image reference.
     * @throws {TypeError} - throw type error on invalid arguments.
     * @param data img data
     * @returns void
     */
    setData(data: Buffer | string): void;
    isLoaded(): boolean;
}
export declare class Signature implements ISignature {
}
export declare class Annotation implements IAnnotation {
}
