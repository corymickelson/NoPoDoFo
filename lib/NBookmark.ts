import {NDocument} from "./NDocument"
import {nopodofo} from "../index"
import {NAction} from "./NAction"
import {NDestination} from "./NDestination"

export interface IBookmarkOutline {
    title: string
    action?: NAction
    destination?: NDestination
    children: IBookmarkOutline[] | []
    next: IBookmarkOutline | []
}

export interface IBookmarkOutlines {
    root: IBookmarkOutline
}

export class NBookmark {
    private outline: nopodofo.Outline | null

    constructor(private parent: NDocument) {
        this.outline = this.parent.getOutlines(false)
    }

}
