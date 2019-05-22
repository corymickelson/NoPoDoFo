import {nopodofo} from "../index";
import {NDocument} from "./NDocument";

export class NRef implements nopodofo.Ref {
    get gennum(): number {
        return this.self.gennum
    }

    get objnum(): number {
        return this.self.objnum
    }

    constructor(private parent: NDocument, private self: nopodofo.Ref) {
    }
}
