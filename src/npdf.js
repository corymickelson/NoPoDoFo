/**
 * @type {npdf}
 */
const npdf = require('bindings')('npdf')

export class Document {
    constructor(file) {
        this._document = new npdf.Document()
        if(file) this._document.load(file)
    }
    load(file) {
        this._document.load(file)
    }
    getPage(index) {
        const page = this._document.getPage(index)
        return new Page(page)
    }
    getPageCount() {
        return this._document.getPageCount()
    }
    mergeDocument(file) {
        this._document.mergeDocument(file)
    }
    deletePage(index) {
        this._document.deletePage(index)
    }
    getVersion() {
        return this._document.getVersion()
    }
    isLinearized() {
        return this._document.isLinearized()
    }
    setPassword(pwd) {
        this._document.setPassword(pwd)
    }
    write(dest) {
        this._document.write(dest)
    }
}
export class Page {
    constructor(_page) {
        this._page = _page;
    }
    getRotation() {
    }
}
export class Transform {
    /**
     *
     * @param {string} file
     */
    constructor(file) {
        this.document = new npdf.Document()
        this.document.load(file)
    }

}
