import { Page } from "./page";
import { __mod } from "./document";

export class ContentsTokenizer {
    private _instance:any
    constructor(page:Page) {
        this._instance = new __mod.ContentsTokenizer((page as any)._instance)
    }
    tokens(): Array<string> {
        return this._instance.readAll()
    }
}