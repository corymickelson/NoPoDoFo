import {nopodofo, NPDFColorSpace} from "../index"
import {NDocument} from "./NDocument";

export class NImage implements nopodofo.Image {
    constructor(private parent: NDocument, private self: nopodofo.Image) {

    }

    get height(): number {
        return this.self.height
    }

    get width(): number {
        return this.self.width
    }

    setInterpolate(v: boolean): void {
        return this.self.setInterpolate(v)
    }

    setColorSpace(colorSpace: NPDFColorSpace): void {
        this.self.setColorSpace(colorSpace)
    }

    setICCProfile(input: Buffer, colorComponent: number, alt: NPDFColorSpace): void {
        this.self.setICCProfile(input, colorComponent, alt)
    }

    setChromaKeyMask(r: number, g: number, b: number, threshold: number): void {
    }

    setSoftMask(img: nopodofo.Image): void {
    }
}
