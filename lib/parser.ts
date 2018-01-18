import { Page } from "./page";
import { __mod, Document } from "./document";

/**
 * This class is a parser for content streams in PDF documents.
 * PoDoFo::PdfContentsTokenizer is currently a work in progress.
 */
export class ContentsTokenizer {
    private _instance:any
    constructor(page:Page, doc:Document) {
        this._instance = new __mod.ContentsTokenizer((page as any)._instance, (doc as any)._instance)
    }

    /**
     * Read all text content from the page.
     * @returns {Array<string>}
     */
    readAllContent(): Array<string> {
        return this._instance.readAll()
    }
}