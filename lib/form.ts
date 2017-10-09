import {IDocument, __mod} from './document'
import {IObj} from './object'

export class Form {
    _instance:any

    get needAppearances() {return this._instance.needAppearances}
    set needAppearances(value:boolean) {this._instance.needAppearances = value}

    constructor(document:IDocument) {
        this._instance = new __mod.Form(document._instance)
    }

    getObject():IObj {
        return this._instance.getObject()
    }
}