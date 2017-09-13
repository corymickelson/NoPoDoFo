declare const mod: any;
interface IPage {
    getRotation(): number;
    getNumFields(): number;
    getField(index: number): IField;
    setRotation(degree: number): void;
    getBottom(): number;
    setBottom(value: number): void;
    getLeft(): number;
    setLeft(value: number): void;
    getWidth(): number;
    setWidth(value: number): void;
    getHeight(): number;
    setHeight(value: number): void;
    addImg(file: string, dx: number, dy: number, scaleX: number, scaleY: number): void;
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
    setFieldValue(fieldIndex: number, value: number | string): void;
    setFieldAlternateName(fieldIndex: number, value: string): void;
    setFieldMappingName(fieldIndex: number, value: string): void;
    setFieldRequired(fieldIndex: number, value: boolean): void;
    getFieldIndex(fieldName: string): number;
}
interface IField {
}
interface IDocument {
    load(file: string): void;
    getPageCount(): number;
    getPage(pageN: number): Page;
    mergeDocument(doc: string): void;
    deletePage(pageIndex: number): void;
    getVersion(): number;
    isLinearized(): boolean;
    setPassword(password: string): void;
    write(file: string): void;
}
declare class Document implements IDocument {
    load(file: string): void;
    getPageCount(): number;
    getPage(pageN: number): Page;
    mergeDocument(doc: string): void;
    deletePage(pageIndex: number): void;
    getVersion(): number;
    isLinearized(): boolean;
    setPassword(password: string): void;
    write(file: string): void;
    private _instance;
    constructor(file: string);
}
declare class Page implements IPage {
    getRotation(): number;
    getNumFields(): number;
    getField(index: number): IField;
    setRotation(degree: number): void;
    getBottom(): number;
    setBottom(value: number): void;
    getLeft(): number;
    setLeft(value: number): void;
    getWidth(): number;
    setWidth(value: number): void;
    getHeight(): number;
    setHeight(value: number): void;
    addImg(file: string, dx: number, dy: number, scaleX: number, scaleY: number): void;
    getFields(): [{
        name: string;
        alternateName: string;
        mappingName: string;
        required: boolean;
        readOnly: boolean;
        value: (string | number);
        maxLen?: number;
        multiLine?: boolean;
        caption?: string;
        type: string;
        selected?: string;
    }];
    setFieldValue(fieldIndex: number, value: number | string): void;
    setFieldAlternateName(fieldIndex: number, value: string): void;
    setFieldMappingName(fieldIndex: number, value: string): void;
    setFieldRequired(fieldIndex: number, value: boolean): void;
    getFieldIndex(fieldName: string): number;
}
