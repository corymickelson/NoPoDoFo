import {EncryptOption, nopodofo, NPDFExternal, ProtectionSummary} from "../index"
import {NDocument} from "./NDocument"
import Encrypt = nopodofo.Encrypt;

export class NEncrypt implements nopodofo.Encrypt {
    get user(): string {
        return this.self.user
    }

    set user(value: string) {
        this.self.user = value
    }

    get protections(): ProtectionSummary {
        return this.self.protections
    }

    set protections(value: ProtectionSummary) {
        this.self.protections = value
    }

    get owner(): string {
        return this.self.owner
    }

    set owner(value: string) {
        this.self.owner = value
    }

    get keyLength(): number {
        return this.self.keyLength
    }

    set keyLength(value: number) {
        this.self.keyLength = value
    }

    get encryptionKey(): string {
        return this.self.encryptionKey
    }

    set encryptionKey(value: string) {
        this.self.encryptionKey = value
    }

    constructor(private parent: NDocument, private self: nopodofo.Encrypt) {
    }

    isAllowed(action: "Copy" | "Print" | "Edit" | "EditNotes" | "FillAndSign" | "Accessible" | "DocAssembly" | "HighPrint"): boolean {
        return this.self.isAllowed(action)
    }
}
