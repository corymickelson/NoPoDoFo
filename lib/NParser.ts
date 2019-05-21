import {nopodofo} from "../index"
import {NDocument} from "./NDocument"

export class NParser {
    constructor(private parent: NDocument, private self: nopodofo.ContentsTokenizer) {
    }

    read(): Promise<string> {
        return new Promise<string>((resolve, reject) => {
            const res = this.self.read((err, data) => err ? reject(err) : resolve(data))
        })
    }

    readSync(): Iterator<string> {
        return this.self.readSync()
    }
}
