import {nopodofo} from "../index";
import {NDocument} from "./NDocument";

export class NData implements nopodofo.Data {
    constructor(private parent: NDocument, private self: nopodofo.Data) {
    }

    get value(): string {
        return this.self.value
    }

    write(output: string): void {
        this.self.write(output)
    }

}
