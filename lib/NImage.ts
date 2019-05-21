import {nopodofo} from "../index"
import {NDocument} from "./NDocument";

export class NImage implements nopodofo.Image {
    constructor(private parent: NDocument, private self: nopodofo.Image) {

    }

    get height(): number {
        return this.self.height
    }

    get width(): number {
        return this.self.width
    }

    setInterpolate(v: boolean): void {
        return this.self.setInterpolate(v)
    }
}
