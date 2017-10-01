const mod = require('bindings')('npdf')
import {IDocument} from './document'
import {IPDObject} from './object'

export class Form {
    private _instance:any
    private _needAppearances:boolean

    get needAppearances() {return this._needAppearances}
    set needAppearances(value:boolean) {this._needAppearances = value}

    constructor(document:IDocument) {
        this._instance = new mod.Form(document)
    }

    getObject():IPDObject {
        return this._instance.getObject()
    }
}