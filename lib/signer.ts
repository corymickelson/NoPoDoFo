import { __mod, Document } from './document'
import { SignatureField } from "./field";
import { existsSync } from "fs";
import { Data } from './data';
import { Annotation } from './annotation';

/**
 * The Signer class binds PoDoFo::PdfSignOutputDevice
 */
export class Signer {
    private _instance: any
    constructor(doc: Document, output?: string) {
        if (output) {
            this._instance = new __mod.Signer((doc as any)._instance, output)

        } else
            this._instance = new __mod.Signer((doc as any)._instance)
    }

    signSync(signature: string , output?:string): Buffer {
        return this._instance.signSync(signature, output)
    }

    sign(signature:string, output:string, cb:(e:Error, d:Buffer) => void): void {
        this._instance.sign(cb, signature, output)
    }

    setField(field:SignatureField) {
        if(!(field as any)._instance) {
            throw Error("field undefined")
        }
        this._instance.setField((field as any)._instance)
    }

    getField(): SignatureField {
        return new SignatureField(this._instance.getField())
    }
}

export function signature(certfile: string, pkeyfile: string, password: string = ''): string {
    if (!existsSync(certfile) || !existsSync(pkeyfile)) {
        throw Error("One or both files not found")
    }
    return __mod.signature(certfile, pkeyfile, password)
}