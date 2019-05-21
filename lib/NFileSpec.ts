import {nopodofo} from "../index"
import {NDocument} from "./NDocument"

export class NFileSpec implements nopodofo.FileSpec {
    constructor(private parent: NDocument, private self: nopodofo.FileSpec) {
    }

    get name(): string {
        return this.self.name
    }

    getContents(): Buffer | undefined {
        return this.self.getContents()
    }
}
