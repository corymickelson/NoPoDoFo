import {nopodofo, NPDFFontMetrics} from "../index"
import {NDocument} from "./NDocument"
import {NObject} from "./NObject"
import {NEncoding} from "./NEncoding";

export class NFont {
    get wordSpace(): number {
        return this.self.wordSpace
    }

    set wordSpace(value: number) {
        this.self.wordSpace = value
    }

    get underline(): boolean {
        return this.self.underline
    }

    set underline(value: boolean) {
        this.self.underline = value
    }

    get strikeOut(): boolean {
        return this.self.strikeOut
    }

    set strikeOut(value: boolean) {
        this.self.strikeOut = value
    }

    get size(): number {
        return this.self.size
    }

    set size(value: number) {
        this.self.size = value
    }

    get scale(): number {
        return this.self.scale
    }

    set scale(value: number) {
        this.self.scale = value
    }

    get object(): NObject {
        return new NObject(this.parent, this.self.object)
    }

    set object(value: NObject) {
        this.self.object = (value as any).self
    }

    get identifier(): string {
        return this.self.identifier
    }

    set identifier(value: string) {
        this.self.identifier = value
    }

    get charSpace(): number {
        return this.self.charSpace
    }

    set charSpace(value: number) {
        this.self.charSpace = value
    }

    constructor(private parent: NDocument, private self: nopodofo.Font) {

    }

    embed(): void {
        this.self.embed()
    }

    embedSubsetFont(): void {
        this.self.embedSubsetFont()
    }

    getEncoding(): NEncoding {
        return new NEncoding(this.parent, this.self.getEncoding())
    }

    getMetrics(): NPDFFontMetrics {
        return this.self.getMetrics()
    }

    isBold(): boolean {
        return this.self.isBold()
    }

    isItalic(): boolean {
        return this.self.isItalic()
    }

    isSubsetting(): boolean {
        return this.self.isSubsetting()
    }

    stringWidth(v: string): number {
        return this.self.stringWidth(v)
    }

    write(content: string, stream: nopodofo.Stream): void {
        this.self.write(content, stream)
    }

}
