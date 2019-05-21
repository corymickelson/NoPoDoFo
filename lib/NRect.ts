import {nopodofo} from "../index"
import {NDocument} from "./NDocument";

export class NRect implements nopodofo.Rect {
    get height(): number {
        return this.self.height;
    }

    set height(value: number) {
        this.self.height = value;
    }

    get width(): number {
        return this.self.width;
    }

    set width(value: number) {
        this.self.width = value;
    }

    get bottom(): number {
        return this.self.bottom;
    }

    set bottom(value: number) {
        this.self.bottom = value;
    }

    get left(): number {
        return this.self.left;
    }

    set left(value: number) {
        this.self.left = value;
    }

    private self: nopodofo.Rect;

    constructor(private parent: NDocument, left?: number | nopodofo.Rect, bottom?: number, width?: number, height?: number) {
        if (left instanceof nopodofo.Rect) {
            this.self = left
        } else if (left && bottom && width && height) {
            this.self = new nopodofo.Rect(left, bottom, width, height)
        } else {
            this.self = new nopodofo.Rect()
        }
    }

    intersect(rect: NRect): void {
        this.self.intersect(rect.self)
    }

}
