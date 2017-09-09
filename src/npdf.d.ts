declare namespace npdf {
    class Document {
        constructor()
        load(file:string)
        getPageCount(): number
        getPage(pageN: number): Page
        getVersion(): number
        isLinearized():boolean
        setPassword(password:string)
        write(file:string)
        write(): Buffer
    }
    class Page {
        constructor()
        getRotation(): number
        getNumFields(): number
        getField(index: number): Field
        setRotation(degree: number)
    }

    class Field {}
}
