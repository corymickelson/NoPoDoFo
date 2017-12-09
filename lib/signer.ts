import {__mod, Document} from './document'
import {SignatureField} from "./field";
import {existsSync} from "fs";
import { Data } from './data';

/**
 * The Signer class binds PoDoFo::PdfSignOutputDevice
 */
export class Signer {
    private _instance:any
    get signatureSize(): number {
        return this._instance.signatureSize
    }
    set signatureSize(i: number) {
        if(i > 0) this._instance.signatureSize = i
        else {
            throw Error("size must be greater than 0")
        }
    }
    get length(): number {
        return this._instance.length
    }

    constructor(doc:Document, output?:string) {
        if(output && (output as any) instanceof String) {
            if(existsSync(output)) {
                throw Error("Do not write signed doc to the same path as the loaded document")
            } else {
                this._instance = new __mod.Signer((doc as any)._instance, output)
            }
        } else
            this._instance = new __mod.Signer((doc as any)._instance)
    }

    setSignature(data:string): void {

    }
    hasSignaturePosition(): boolean {
        return this._instance.hasSignaturePosition()
    }
    getBeacon(): Data {
        return new Data(this._instance.getBeacon())
    }
    adjustByteRange(): void {
        this._instance.adjustByteRange()
    }
    write(data:string): void {
        this._instance.write(data)
    }
    read(len:number):Buffer {
        return this._instance.read(len)
    }
    seek(pos:number): void {
        this._instance.seek(pos)
    }
    flush(): void {
        this._instance.flush()
    }
    sign(signature:string|Buffer): Buffer {
        return this._instance.sign(signature)
    }
}

export function signature(certfile:string, pkeyfile:string, password:string = ''): string {
    if(!existsSync(certfile) || !existsSync(pkeyfile)) {
        throw Error("One or both files not found")
    }
   return __mod.signature(certfile, pkeyfile, password)
}