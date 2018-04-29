import { BaseDocument, __mod } from "./base-document";
import { IEncrypt } from "./encrypt";
import { NPDFInternal } from "./object";

export enum NPDFVersion {
    Pdf11,
    Pdf12,
    Pdf13,
    Pdf14,
    Pdf15,
    Pdf16,
    Pdf17,
}

export enum NPDFWriteMode {
    Default = 0x01,
    Compact = 0x02
}

export class StreamDocument extends BaseDocument {
    private _instance: NPDFInternal
    constructor(name:string, version: NPDFVersion, writer: NPDFWriteMode, encrypt?: IEncrypt) {
        super()
        this._instance = new __mod.StreamDocument(name, version, writer, encrypt || null)
        this.setInternal(this._instance)
    }

    /**
     * @description Calls PdfStreamedDocument::Close and emits the close event.
     */
    close(): void {
        this._instance.close()
        this.emit('close')
    }
}