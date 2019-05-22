import {Callback, nopodofo} from "../index";
import {NDocument} from "./NDocument";

export class NStream {
    constructor(private parent: NDocument, private self: nopodofo.Stream) {
    }

    append(data: string | Buffer): void {
        this.self.append(data)
    }

    beginAppend(): void {
        this.self.beginAppend()
    }

    copy(filtered?: boolean): Buffer {
        if (filtered) return this.self.copy(filtered)
        else return this.self.copy()
    }

    endAppend(): void {
        this.self.endAppend()
    }

    inAppendMode(): boolean {
        return this.self.inAppendMode()
    }

    set(data: string | Buffer): void {
        this.self.set(data)
    }

    write(output?: string): Promise<Buffer | string> {
        if (output) {
            return new Promise<Buffer | string>((resolve, reject) =>
                this.self.write(output, (err, data) => err ? reject(err) : resolve(data)))
        } else {
            return new Promise<Buffer | string>((resolve, reject) =>
                this.self.write((err, data) => err ? reject(err) : resolve(data)))
        }
    }
}
