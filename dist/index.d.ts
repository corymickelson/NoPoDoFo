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
interface IField {
    getType(): string;
    getFieldName(): string;
    getAlternateName(): string;
    getMappingName(): string;
    isRequired(): boolean;
    setRequired(required: boolean): void;
    setAlternateName(name: string): void;
    setMappingName(name: string): void;
}
interface IPainter {
    setPage(page: IPage): void;
    finishPage(): void;
    drawText(): void;
    drawImage(imgFile: string): void;
    getPrecision(): number;
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
    getFieldIndex(fieldName: string): number;
}
