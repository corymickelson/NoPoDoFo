import {nopodofo, NPDFDestinationType} from "../index"
import {NDocument} from "./NDocument"
import {NPage} from "./NPage"

export class NDestination {

    constructor(private parent: NDocument, public self: nopodofo.Destination) {
    }

    get page(): NPage {
        return new NPage(this.parent, this.self.page)
    }

    get type(): NPDFDestinationType {
        return this.self.type
    }
}
