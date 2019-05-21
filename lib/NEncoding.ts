import {nopodofo} from "../index";
import {NDocument} from "./NDocument";
import {NObject} from "./NObject";

export class NEncoding {
    constructor(private parent: NDocument, private self: nopodofo.Encoding) {
    }

    addToDictionary(target: NObject): void {
        this.self.addToDictionary((target as any).self)
    }

    convertToEncoding(content: string, font: nopodofo.Font): Buffer {
        return this.self.convertToEncoding(content, font)
    }

    convertToUnicode(content: string, font: nopodofo.Font): string {
        return this.self.convertToUnicode(content, font)
    }

}
