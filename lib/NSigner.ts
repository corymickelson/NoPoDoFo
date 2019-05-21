import {nopodofo} from "../index"
import {NDocument} from "./NDocument"
import {NSignatureField} from "./NSignatureField"

export class NSigner {
    get signatureField(): NSignatureField {
        return new NSignatureField(this.parent, this.self.signatureField)
    }

    set signatureField(value: NSignatureField) {
        this.self.signatureField = (value as any).self
    }

    constructor(private parent: NDocument, private self: nopodofo.Signer) {
    }

    readCert(cert: string | Buffer, opts?: { password?: string, pKey?: string | Buffer }): Promise<number> {
        return new Promise<number>((resolve, reject) =>
            this.self.loadCertificateAndKey(cert, opts, (err, data) => err ? reject(err) : resolve(data as number)))
    }


    write(minSignatureSize: number): Promise<string | Buffer> {
        return new Promise<string | Buffer>((resolve, reject) =>
            this.self.write(minSignatureSize, (err, data) => err ? reject(err) : resolve(data)))
    }

}
