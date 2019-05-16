import {NDocument} from './NDocument'
import {nopodofo, NPDFOutlineFormat} from '../index'
import {NDestination} from "./NDestination";
import {NAction} from "./NAction";


export class NOutlineItem {

    /**
     * Get the previous outline item
     */
    get prev(): NOutlineItem | null {
        return this.getItem('prev')
    }

    /**
     * Get the next outline item
     */
    get next(): NOutlineItem | null {
        return this.getItem('next')
    }

    /**
     * Get the first outline item that is a child of this item
     */
    get first(): NOutlineItem | null {
        return this.getItem('first')
    }

    /**
     * Get the last outline item that is a chile of this item
     */
    get last(): NOutlineItem | null {
        return this.getItem('last')
    }

    get destination(): NDestination | null {
        const item = (this.self as nopodofo.Outline)
        if (item.destination) {
            return new NDestination(this.parent, item.destination)
        }
        return null
    }

    set destination(v: NDestination | null) {
        (this.self as nopodofo.Outline).destination = (v as NDestination).self
    }

    get action(): NAction | null {
        const item = (this.self as nopodofo.Outline)
        if (item.action) {
            return new NAction(this.parent, item.action)
        } else {
            return null
        }
    }

    set action(v: NAction | null) {
        if (!v) {

        }
        (this.self as nopodofo.Outline).action = (v as NAction).self
    }

    get title(): string {
        const item = (this.self as nopodofo.Outline)
        return item.title
    }

    set title(v: string) {
        (this.self as nopodofo.Outline).title = v
    }

    get textFormat(): NPDFOutlineFormat {
        const item = (this.self as nopodofo.Outline)
        return item.textFormat
    }

    set textFormat(v: NPDFOutlineFormat) {
        (this.self as nopodofo.Outline).textFormat = v
    }

    get textColor(): nopodofo.Color {
        const item = (this.self as nopodofo.Outline)
        return item.textColor
    }

    set textColor(v: nopodofo.Color) {
        (this.self as nopodofo.Outline).textColor = v
    }

    protected constructor(private parent: NDocument, protected self?: nopodofo.Outline) {
        if (!this.self) this.self = this.parent.getOutlines(true) as nopodofo.Outline
    }

    private getItem(m: 'last' | 'first' | 'next' | 'prev'): NOutlineItem | null {
        const item = (this.self as nopodofo.Outline)[m]
        if (item) {
            return new NOutlineItem(this.parent, item)
        } else {
            return null
        }
    }

}

export class NOutlines extends NOutlineItem {
    constructor(parent: NDocument) {
        super(parent)
    }
}
