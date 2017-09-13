declare namespace NPdf {
    export class Document {
        constructor()

        load(file: string)

        getPageCount(): number

        getPage(pageN: number): Page

        mergeDocument(doc: string)

        deletePage(pageIndex: number)

        getVersion(): number

        isLinearized(): boolean

        setPassword(password: string)

        write(file: string)
    }

    export class Page {
        constructor()

        getRotation(): number

        getNumFields(): number

        getField(index: number): Field

        setRotation(degree: number)

        getBottom(): number

        setBottom(value: number)

        getLeft(): number

        setLeft(value: number)

        getWidth(): number

        setWidth(value: number)

        getHeight(): number

        setHeight(value: number)

        addImg(file: string, dx: number, dy: number, scaleX: number, scaleY: number)

        getFields(): [{
            name: string, alternateName: string, mappingName: string, required: boolean, readOnly: boolean, value: string | number,
            maxLen?: number, multiLine?: boolean, caption?: string, type: string, selected?: string
        }]

        setFieldValue(fieldIndex: number, value: number | string)

        setFieldAlternateName(fieldIndex: number, value: string)

        setFieldMappingName(fieldIndex: number, value: string)

        setFieldRequired(fieldIndex: number, value: boolean)

        getFieldIndex(fieldName: string): number
    }

    class Field {
    }
}
