import {nopodofo, NPDFBlendMode, NPDFRenderingIntent} from "../index"
import {NDocument} from "./NDocument"

export class NExtGState implements nopodofo.ExtGState {
    constructor(private parent: NDocument, private self: nopodofo.ExtGState) {
    }

    setBlendMode(mode: NPDFBlendMode): void {
        this.self.setBlendMode(mode)
    }

    setFillOpacity(v: number): void {

        this.self.setFillOpacity(v)
    }

    setFillOverprint(v: boolean): void {
        this.self.setFillOverprint(v)
    }

    setFrequency(v: number): void {
        this.self.setFrequency(v)
    }

    setNonZeroOverprint(v: boolean): void {
        this.self.setNonZeroOverprint(v)
    }

    setOverprint(v: boolean): void {
        this.self.setOverprint(v)
    }

    setRenderingIntent(intent: NPDFRenderingIntent): void {
        this.self.setRenderingIntent(intent)
    }

    setStrokeOpacity(v: number): void {
        this.self.setStrokeOpacity(v)
    }

    setStrokeOverprint(v: boolean): void {
        this.self.setStrokeOverprint(v)
    }
}
