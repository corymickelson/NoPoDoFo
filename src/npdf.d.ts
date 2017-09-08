declare namespace npdf {
    class pdfDocument {
        constructor()
        load(file:string)
        getPageCount(): number
        getPage(pageN: number)
        getVersion(): number
        isLinearized():boolean
        setPassword(password:string)
        write(file:string)
        write(): Buffer
    }
}
