import {nopodofo} from "../index"
import {NDocument} from "./NDocument"
import {NObject} from "./NObject"

export class NXObject {
    constructor(private parent: NDocument, public self: nopodofo.XObject) {
    }

    get contents(): NObject {
        return new NObject(this.parent, this.self.contents)
    }

    get contentsForAppending(): NObject {
        return new NObject(this.parent, this.self.contentsForAppending)
    }

    get pageMediaBox(): nopodofo.Rect {
        return this.self.pageMediaBox
    }

    get reference(): nopodofo.Ref {
        return this.self.reference
    }

    get resources(): NObject {
        return new NObject(this.parent, this.self.resources)
    }

}
