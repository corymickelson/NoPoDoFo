export interface IPage {
    getRotation(): number;
    getNumFields(): number;
    setRotation(degree: number): void;
    getFields(): [{
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
    }];
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
    drawImage(imgFile: string, x: number, y: number, width: number, height: number): void;
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
}
export interface IAnnotation {
}
export interface ISignature {
}
export declare class Document implements IDocument {
    private _instance;
    constructor(file: string);
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
export declare class Field implements IField {
    private _instance;
    constructor(page: IPage, fieldIndex: number);
    getType(): string;
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
export declare class Painter implements IPainter {
    private _instance;
    constructor();
    setPage(page: IPage): void;
    finishPage(): void;
    drawText(): void;
    drawImage(imgFile: string, x: number, y: number, width: number, height: number): void;
    getPrecision(): number;
}
export declare class Rect implements IRect {
    private _instance;
    constructor(page: IPage);
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
}
export declare class Signature implements ISignature {
}
export declare class Annotation implements IAnnotation {
}
