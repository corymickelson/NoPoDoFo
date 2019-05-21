import {nopodofo} from '../'
import {NDocument} from "./NDocument"
import {NObject} from "./NObject"

export class NAction {
    get uri(): string|undefined {
        return this.self.uri
    }

    set uri(value: string|undefined) {
        this.self.uri = value;
    }

    get script(): string {
        if (!this.self.script) {
            return ''
        } else {
            return this.self.script
        }
    }

    set script(value: string) {
        this.self.script = value
    }

    constructor(private parent: NDocument, private self: nopodofo.Action) {
    }

    addToDictionary(dictionary: nopodofo.Dictionary): void {
        this.self.addToDictionary(dictionary)
    }

    getObject(): NObject {
        const o = this.self.getObject()
        return new NObject(this.parent, o)
    }
}
