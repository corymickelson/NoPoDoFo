import {
    nopodofo,
    NPDFVersion,
    NPDFWriteMode,
    NPDFPageMode,
    NPDFPageLayout,
    NPDFInfo,
    NPDFDestinationFit,
    ProtectionOption,
    NPDFCreateFontOpts,
    Callback
} from "../index"

/**
 * @class NDocument
 */
export class NDocument implements nopodofo.Base {

    // start StreamDocument
    close(): string | Buffer {
        if (!((this.base as any) instanceof nopodofo.StreamDocument)) {
            throw TypeError('must be an instance of NoPoDoFo.StreamDocument')
        }
        const output = (this.base as nopodofo.StreamDocument).close()
        this.finish()
        return output
    }
    // end StreamDocument
    // start MemDocument
    get encrypt(): nopodofo.Encrypt {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        const encrypt = (this.base as nopodofo.Document).encrypt
        if (!encrypt) {
            this.children.push(encrypt)
        }
        return encrypt
    }
    private _trailer?: nopodofo.Object
    get trailer(): nopodofo.Object {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        if (this._trailer) {
            return this._trailer
        }
        const trailer = (this.base as nopodofo.Document).trailer
        this.children.push(trailer)
        return trailer
    }
    private _catalog?: nopodofo.Object
    get catalog(): nopodofo.Object {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        if (this._catalog) {
            return this._catalog
        }
        const catalog = (this.base as nopodofo.Document).catalog
        this.children.push(catalog)
        return catalog
    }
    load(file: string | Buffer, opts: { forUpdate?: boolean; password?: string; }, cb: Callback<void>): void;
    load(file: string | Buffer, cb: Callback<nopodofo.Document>): void;
    load(file: any, opts: any, cb?: any) {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
    }
    setPassword(pwd: string): void {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        (this.base as nopodofo.Document).setPassword(pwd)
    }
    getFont(name: string): nopodofo.Font {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        const font = (this.base as nopodofo.Document).getFont(name)
        this.children.push(font)
        return font
    }
    listFonts(): { name: string; id: string; file: string; }[] {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        const fonts = (this.base as nopodofo.Document).listFonts()
        this.children.push(fonts)
        return fonts
    }
    splicePages(startIndex: number, count: number): void {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        (this.base as nopodofo.Document).splicePages(startIndex, count)
    }
    insertPages(fromDoc: nopodofo.Document, startIndex: number, count: number): number {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        return (this.base as nopodofo.Document).insertPages(fromDoc, startIndex, count)
    }
    write(destination: string | Callback<Buffer>, cb?: Callback<string>): void {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        (this.base as nopodofo.Document).write(destination, cb)
        this.finish()
    }
    hasSignatures(): boolean {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        return (this.base as nopodofo.Document).hasSignatures()
    }
    getSignatures(): nopodofo.SignatureField[] {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        const signatures = (this.base as nopodofo.Document).getSignatures()
        this.children.push(signatures)
        return signatures
    }
    append(doc: nopodofo.Document | nopodofo.Document[]): void {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        (this.base as nopodofo.Document).append(doc)
    }
    insertExistingPage(memDoc: nopodofo.Document, index: number, insertIndex: number): number {
        if (!((this.base as any) instanceof nopodofo.Document)) {
            throw TypeError('must be an instance of NoPoDoFo.Document')
        }
        return (this.base as nopodofo.Document).insertExistingPage(memDoc, index, insertIndex)
    }

    // end MemDocument
    private _body: nopodofo.Object[] = []
    private _form?: nopodofo.Form
    get form(): nopodofo.Form {
        if (!this._form) {
            this._form = this.base.form
        }
        this.children.push(this._form)
        return this._form
    }
    get body(): nopodofo.Object[] {
        if (!this._body) {
            this._body = this.base.body
        }
        this.children.push(this._body)
        return this._body
    }
    get version(): NPDFVersion {
        return this.base.version
    }
    get pageMode(): NPDFPageMode {
        return this.base.pageMode
    }
    get pageLayout(): NPDFPageLayout {
        return this.base.pageLayout
    }
    get printingScale(): string {
        return this.base.printingScale
    }
    get baseUri(): string {
        return this.base.baseUri
    }
    get language(): string {
        return this.base.language
    }
    get info(): NPDFInfo {
        return this.base.info
    }
    getPageCount(): number {
        return this.base.getPageCount()
    }
    getPage(n: number): nopodofo.Page {
        const page = this.base.getPage(n)
        this.children.push(page)
        return page
    }
    hideToolbar(): void {
        this.base.hideToolbar()
    }
    hideMenubar(): void {
        this.base.hideMenubar()
    }
    hideWindowUI(): void {
        this.base.hideWindowUI()
    }
    fitWindow(): void {
        this.base.fitWindow()
    }
    centerWindow(): void {
        this.base.centerWindow()
    }
    displayDocTitle(): void {
        this.base.displayDocTitle()
    }
    useFullScreen(): void {
        this.base.useFullScreen()
    }
    attachFile(file: string): void {
        this.base.attachFile(file)
    }
    insertPage(rect: nopodofo.Rect, index: number): nopodofo.Page {
        const page = this.base.insertPage(rect, index)
        this.children.push(page)
        return page
    }
    isLinearized(): boolean {
        return this.base.isLinearized()
    }
    getWriteMode(): NPDFWriteMode {
        return this.base.getWriteMode()
    }
    isAllowed(perm: ProtectionOption): boolean {
        return this.base.isAllowed(perm)
    }
    createFont(opts: NPDFCreateFontOpts): nopodofo.Font {
        const font = this.base.createFont(opts)
        this.children.push(font)
        return font
    }
    createFontSubset(opts: NPDFCreateFontOpts): nopodofo.Font {
        const font = this.base.createFont(opts)
        this.children.push(font)
        return font
    }
    getOutlines(create?: boolean, root?: string): nopodofo.Outline|null {
        const outlines = this.base.getOutlines(create, root)
        if (outlines !== null) {
            this.children.push(outlines)
        }
        return outlines
    }
    getObject(ref: nopodofo.Ref): nopodofo.Object {
        const obj = this.base.getObject(ref)
        this.children.push(obj)
        return obj
    }
    getNames(create: boolean): nopodofo.Object|null {
        const names = this.base.getNames(create)
        if (names !== null) {
            this.children.push(names)
        }
        return names
    }
    createXObject(rect: nopodofo.Rect): nopodofo.XObject {
        const x = this.base.createXObject(rect)
        this.children.push(x)
        return x
    }
    createPage(rect: nopodofo.Rect): nopodofo.Page {
        const page = this.base.createPage(rect)
        this.children.push(page)
        return page
    }
    createPages(rects: nopodofo.Rect[]): number {
        return this.base.createPages(rects)
    }
    getAttachment(fileName: string): nopodofo.FileSpec {
        const n = this.base.getAttachment(fileName)
        this.children.push(n)
        return n
    }
    addNamedDestination(page: nopodofo.Page, destination: NPDFDestinationFit, name: string): void {
        const d = this.base.addNamedDestination(page, destination, name)
        this.children.push(d)
        return d
    }
    children: any[] = []
    get memory(): nopodofo.Document|undefined {
        if(this.base instanceof  nopodofo.Document) {
            return this.base
        } else {
            return undefined
        }
    }
    get stream(): nopodofo.StreamDocument|undefined {
        if(this.base instanceof nopodofo.StreamDocument) {
            return this.base
        } else {
            return undefined
        }
    }
    private constructor(private base: nopodofo.Base) {
    }
    static async from(
        src: string | Buffer,
        opts: { forUpdate?: boolean, password?: string } = { forUpdate: false, password:"" })
        : Promise<NDocument> {
        const doc = new nopodofo.Document()
        await new Promise((resolve, reject) => doc.load(src, opts, (e:Error) => e ? reject(e) : resolve(doc)))
        return new NDocument(doc)
    }
    static to(
        dest?: string,
        opts?: { version: NPDFVersion, writer: NPDFWriteMode, encrypt?: nopodofo.Encrypt })
        : NDocument {
        const doc = new nopodofo.StreamDocument(dest, opts)
        return new NDocument(doc)
    }
    /**
     * @description Set all children of this instance to undefined so they may be collected by the garbage collector
     */
    private finish() {
        console.log(`clearing ${this.children.length} items`)
        this.children.forEach(c => {
            c = undefined
        })
    }
}

