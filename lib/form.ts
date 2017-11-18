import {IDocument, __mod} from './document'
import {Obj} from "./object";

export class Form {
    private _instance:any

    get needAppearances() {return this._instance.needAppearances}
    set needAppearances(value:boolean) {this._instance.needAppearances = value}

    constructor(document:IDocument) {
        this._instance = new __mod.Form((document as any)._instance)
    }

    getObject():Obj {
        return new Obj(this._instance.getObject())
    }
}