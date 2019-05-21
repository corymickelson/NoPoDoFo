import {nopodofo, NPDFSigFlags} from "../index"
import {NDocument} from "./NDocument"

export class NForm implements nopodofo.Form {
    get needAppearances(): boolean {
        return this.self.needAppearances
    }

    set needAppearances(value: boolean) {
        this.self.needAppearances = value
    }

    get dictionary(): nopodofo.Dictionary {
        return this.self.dictionary
    }

    set dictionary(value: nopodofo.Dictionary) {
        this.self.dictionary = value
    }

    get SigFlags(): NPDFSigFlags {
        return this.self.SigFlags as NPDFSigFlags
    }

    set SigFlags(value: NPDFSigFlags) {
        this.self.SigFlags = value
    }

    get DR(): nopodofo.Dictionary {
        return this.self.DR as nopodofo.Dictionary
    }

    set DR(value: nopodofo.Dictionary) {
        this.self.DR = value
    }

    get DA(): string {
        return this.self.DA as string
    }

    set DA(value: string) {
        this.self.DA = value
    }

    get CO(): nopodofo.Dictionary {
        return this.self.CO as nopodofo.Dictionary
    }

    set CO(value: nopodofo.Dictionary) {
        this.self.CO = value
    }

    constructor(private parent: NDocument, private self: nopodofo.Form) {

    }

}
