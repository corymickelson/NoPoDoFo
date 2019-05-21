import {nopodofo, NPDFAnnotationAppearance, NPDFCertificatePermission} from "../index"
import {NDocument} from "./NDocument"
import {NObject} from "./NObject"
import {NAnnotation} from "./NAnnotation"
import {NXObject} from "./NXObject"
import {NDate} from "./NDate"

export class NSignatureField {
    constructor(private parent: NDocument, private self: nopodofo.SignatureField) {
    }

    get info(): { byteRange: number[], signature: string } {
        return this.self.info
    }

    get obj(): NObject {
        const n = this.self.obj
        return new NObject(this.parent, n)
    }

    get widgetAnnotation(): NAnnotation {
        const n = this.self.widgetAnnotation
        return new NAnnotation(this.parent, n)
    }

    addCertificateReference(perm: NPDFCertificatePermission): void {
        this.self.addCertificateReference(perm)
    }

    ensureSignatureObject(): void {
        this.self.ensureSignatureObject()
    }

    getSignatureObject(): NObject {
        const n = this.self.getSignatureObject()
        return new NObject(this.parent, n)
    }

    setAppearanceStream(xObj: NXObject, appearance: NPDFAnnotationAppearance, name: string): void {
        this.self.setAppearanceStream(xObj.self, appearance, name)
    }

    setCreator(creator: string): void {
        this.self.setCreator(creator)
    }

    setDate(dateTime?: string | NDate): void {
        if (dateTime instanceof NDate) {
            this.self.setDate(dateTime.toString())
        } else if (dateTime as any instanceof String) {
            this.self.setDate(dateTime)
        } else {
            this.self.setDate()
        }
    }

    setFieldName(name: string): void {
        this.self.setFieldName(name)
    }

    setLocation(location: string): void {
        this.self.setLocation(location)
    }

    setReason(reason: string): void {
        this.self.setReason(reason)
    }

}
