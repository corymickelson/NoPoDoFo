import { EventEmitter } from "events";
import { NPDFInternal, Obj } from "./object";
import { IEncrypt } from "./encrypt";
import { PageMode, PageLayout, CreateFontOpts } from "./document";
import { Ref } from "./reference";
import { Page } from "./page";
import { Font } from "./painter";
export const __mod = require('bindings')('npdf')

export class BaseDocument extends EventEmitter {
    private _base: NPDFInternal
    private _encrypt?: IEncrypt
    private _password?: string

    get base(): NPDFInternal {
        return this._base
    }

    set password(value: string) {
        this._password = value;
    }

    get password(): string {
        throw EvalError()
    }

    set pageMode(v: PageMode) {
        this._base.pageMode = v
    }

    get pageMode(): PageMode {
        return this._base.pageMode
    }

    set pageLayout(v: PageLayout) {
        this._base.pageLayout = v
    }

    set printingScale(v: string) {
        this._base.printingScale = v
    }

    set baseURI(v: string) {
        this._base.baseURI = v
    }

    set language(v: string) {
        this._base.language = v
    }
    get encrypt(): IEncrypt {
        if (this._encrypt) return this._encrypt
        else {
            const encrypt = new __mod.Encrypt(this._base)
            this._encrypt = encrypt
            return encrypt
        }
    }
    constructor() {
        super()
    }
    /**
     * @todo Fix this, should not have to pass internal to Base Class
     * @param instance - document instance
     */
    setInternal(instance:NPDFInternal): void {
        this._base = instance
    }
    getPageCount(): number {

        return this._base.getPageCount()
    }

    getPage(pageN: number): Page {
        if (pageN > this.getPageCount() || pageN < 0) {
            throw new RangeError("pageN out of range")
        }

        const page: Page = this._base.getPage(pageN)
        return new Page(page);
    }

    getObject(ref: Ref): Obj {
        if (!ref || ref instanceof Ref === false) {
            throw TypeError()
        }
        else if (ref.isIndirect() === false) {
            throw Error('Document.GetObject is only possible when the object referenced is an indirect object')
        }
        const objInstance = this._base.getObject((ref as any)._base)
        return new Obj(objInstance)
    }

    getObjects(): Array<Obj> {
        const objects: Array<any> = this._base.getObjects()
        return objects.map(value => {
            return new Obj(value)
        })
    }
    getVersion(): number {
        return this._base.getVersion()
    }

    isLinearized(): boolean {
        return this._base.isLinearized()
    }
    /**
     * @desc Creates a PdfFont instance for use in NoPoDoFo generated Pdf Document. Note
     *      it is up to the user to check that the specified font family exists on the system.
     *      For font management use font-manager
     * @see https://github.com/corymickelson/font-manager
     * @param {CreateFontOpts & Object} opts
     * @returns {Font}
     */
    createFont(opts: CreateFontOpts & Object): Font {
        const instance = this._base.createFont(
            opts.fontName,
            opts.hasOwnProperty('bold') ? opts.bold : false,
            opts.hasOwnProperty('italic') ? opts.italic : false,
            opts.hasOwnProperty('encoding') ? opts.encoding : 1,
            opts.hasOwnProperty('embed') ? opts.embed : false,
            opts.hasOwnProperty('fileName') ? opts.fileName : null)
        return new Font(instance)
    }
}