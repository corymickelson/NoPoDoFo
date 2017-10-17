import {IDocument, __mod} from './document'
import {Obj} from "./object";

export class Form {
    _instance:any

    get needAppearances() {return this._instance.needAppearances}
    set needAppearances(value:boolean) {this._instance.needAppearances = value}

    constructor(document:IDocument) {
        this._instance = new __mod.Form(document._instance)
    }

    getObject():Obj {
        return this._instance.getObject()
    }
}