import {
    Callback,
    nopodofo, NPDFActions,
    NPDFCreateFontOpts,
    NPDFDestinationFit,
    NPDFInfo,
    NPDFPageLayout,
    NPDFPageMode,
    NPDFVersion,
    NPDFWriteMode,
    ProtectionOption
} from "../index"
import {NPage} from "./NPage"
import {NXObject} from "./NXObject"
import {NObject} from "./NObject"
import {NPainter} from "./NPainter"
import {NAction} from "./NAction"
import {NDestination} from "./NDestination"
import {NRect} from "./NRect"

/**
 * @class NDocument
 */
export class NDocument {
    get painter(): NPainter {
        if (!this._painter) {
            this._painter = new nopodofo.Painter(this.base)
        }
        return new NPainter(this, this._painter)
    }

    get form(): nopodofo.Form {
        return this.base.form
    }

    get body(): NObject[] {
        return this.base.body.map(i => new NObject(this, i))
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

    createRect(init?: { left: number, bottom: number, width: number, height: number }): NRect {
        return init ? new NRect(this, init.left, init.bottom, init.width, init.height) : new NRect(this)
    }

    createAction(t: NPDFActions): NAction {
        const action = new nopodofo.Action(this.base, t)
        return new NAction(this, action)
    }

    createDestination(page: NPage, fit: NPDFDestinationFit | NRect | { left: number, top: number, zoom: number }, fitArg?: number): NDestination {
        let dest: nopodofo.Destination
        if (fit instanceof NRect) {
            dest = new nopodofo.Destination(page.self, fit.self)
        } else if (typeof (fit) === 'object' && Object.keys(fit).every(k => ['left', 'top', 'zoom'].includes(k))) {
            dest = new nopodofo.Destination(page.self, fit.left, fit.top, fit.zoom)
        } else {
            if (fitArg) dest = new nopodofo.Destination(page.self, (fit as NPDFDestinationFit), fitArg)
            else dest = new nopodofo.Destination(page.self, (fit as NPDFDestinationFit))
        }
        return new NDestination(this, dest)
    }

    getPageCount(): number {
        return this.base.getPageCount()
    }

    getPage(n: number): NPage {
        const page = this.base.getPage(n)
        return new NPage(this, page)
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

    insertPage(rect: NRect, index: number): NPage {
        const page = this.base.insertPage(rect.self, index)
        return new NPage(this, page)
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
        return font
    }
    createFontSubset(opts: NPDFCreateFontOpts): nopodofo.Font {
        const font = this.base.createFont(opts)
        return font
    }
    getOutlines(create?: boolean, root?: string): nopodofo.Outline|null {
        const outlines = this.base.getOutlines(create, root)
        return outlines
    }

    getObject(ref: nopodofo.Ref): NObject {
        const obj = this.base.getObject(ref)
        return new NObject(this, obj)
    }

    getNames(create: boolean): NObject | null {
        const names = this.base.getNames(create)
        if (names) {
            return new NObject(this, names)
        }
        return null
    }

    createXObject(rect: NRect): NXObject {
        const x = this.base.createXObject(rect.self)
        return new NXObject(this, x)
    }

    createPage(rect: NRect): NPage {
        const page = this.base.createPage(rect.self)
        return new NPage(this, page)
    }

    createPages(rects: NRect[]): number {
        return this.base.createPages(rects.map(r => r.self))
    }
    getAttachment(fileName: string): nopodofo.FileSpec {
        const n = this.base.getAttachment(fileName)
        return n
    }
    addNamedDestination(page: nopodofo.Page, destination: NPDFDestinationFit, name: string): void {
        this.base.addNamedDestination(page, destination, name)
    }

    private _painter?: nopodofo.Painter

    protected constructor(protected base: nopodofo.Base) {
    }
    static async from(
        src: string | Buffer,
        opts: { forUpdate?: boolean, password?: string } = { forUpdate: false, password:"" })
        : Promise<NMemoryDocument> {
        const doc = new nopodofo.Document()
        await new Promise((resolve, reject) => doc.load(src, opts, (e:Error) => e ? reject(e) : resolve(doc)))
        return new NMemoryDocument(doc)
    }
    static to(
        dest?: string,
        opts?: { version: NPDFVersion, writer: NPDFWriteMode, encrypt?: nopodofo.Encrypt })
        : NDocument {
        const doc = new nopodofo.StreamDocument(dest, opts)
        return new NStreamDocument(doc)
    }

}

class NMemoryDocument extends NDocument {
    constructor(base: nopodofo.Base) {
        super(base)
    }

    get encrypt(): nopodofo.Encrypt {
        return (this.base as nopodofo.Document).encrypt
    }

    get trailer(): NObject {
        const trailer = (this.base as nopodofo.Document).trailer
        return new NObject(this, trailer)
    }

    get catalog(): NObject {
        const catalog = (this.base as nopodofo.Document).catalog
        return new NObject(this, catalog)
    }

    setPassword(pwd: string): void {
        (this.base as nopodofo.Document).setPassword(pwd)
    }

    getFont(name: string): nopodofo.Font {
        const font = (this.base as nopodofo.Document).getFont(name)
        return font
    }

    listFonts(): { name: string; id: string; file: string; }[] {
        const fonts = (this.base as nopodofo.Document).listFonts()
        return fonts
    }

    splicePages(startIndex: number, count: number): void {
        (this.base as nopodofo.Document).splicePages(startIndex, count)
    }

    insertPages(fromDoc: nopodofo.Document, startIndex: number, count: number): number {
        return (this.base as nopodofo.Document).insertPages(fromDoc, startIndex, count)
    }

    write(destination: string | Callback<Buffer>, cb?: Callback<string>): void {
        (this.base as nopodofo.Document).write(destination, cb)
    }

    hasSignatures(): boolean {
        return (this.base as nopodofo.Document).hasSignatures()
    }

    getSignatures(): nopodofo.SignatureField[] {
        const signatures = (this.base as nopodofo.Document).getSignatures()
        return signatures
    }

    append(doc: nopodofo.Document | nopodofo.Document[]): void {
        (this.base as nopodofo.Document).append(doc)
    }

    insertExistingPage(memDoc: NMemoryDocument, index: number, insertIndex: number): number {
        return (this.base as nopodofo.Document).insertExistingPage(memDoc.base as nopodofo.Document, index, insertIndex)
    }

}

class NStreamDocument extends NDocument {
    constructor(base: nopodofo.Base) {
        super(base)
    }

    close(): string | Buffer {
        const output = (this.base as nopodofo.StreamDocument).close()
        return output
    }
}
