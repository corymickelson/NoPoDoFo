import {nopodofo, NPDFActions} from '../'
import {NDocument} from "./NDocument";

class NAction implements nopodofo.Action {
    get uri(): string|undefined {
        return this._action.uri
    }

    set uri(value: string|undefined) {
        this._action.uri = value;
    }
    get script(): string|undefined {
        return this._action.script;
    }

    set script(value: string|undefined) {
        this._action.script = value;
    }
    constructor(private parent: NDocument, readonly type: NPDFActions) {
        this._action = new nopodofo.Action(parent, type)
        this.parent.children.push(this._action)
    }

    private readonly _action: nopodofo.Action
    private _script: string|undefined;
    private _uri: string|undefined;

    addToDictionary(dictionary: nopodofo.Dictionary): void {
        this._action.addToDictionary(dictionary)
    }

    getObject(): nopodofo.Object {
        const o = this._action.getObject()
        if(o !== null) {
            this.parent.children.push(o)
        }
        return o
    }
}
