declare namespace npdf {
    class Document {
        constructor()

        load(file: string)

        getPageCount(): number

        getPage(pageN: number): Page

        getVersion(): number

        isLinearized(): boolean

        setPassword(password: string)

        write(file: string)
    }

    class Page {
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
    }

    class Field {
    }
}
