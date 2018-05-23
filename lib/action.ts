import {IDocument} from "./document"
import {IDictionary, IObj} from "./object";

export enum NPDFActions {
    GoTo = 0,
    GoToR,
    GoToE,
    Launch,
    Thread,
    URI,
    Sound,
    Movie,
    Hide,
    Named,
    SubmitForm,
    ResetForm,
    ImportData,
    JavaScript,
    SetOCGState,
    Rendition,
    Trans,
    GoTo3DView,
    RichMediaExecute,
    Unknown = 0xff
}
export enum NPDFMouseEvent {
    up,
    down,
    enter,
    exit
}
export enum NPDFPageEvent {
    open,
    close,
    visible,
    invisible
}
export interface IAction {
    new(type:NPDFActions, doc:IDocument): void
    type: NPDFActions
    uri?: string
    script?: string

    getObject(): IObj
    addToDictionary(dictionary: IDictionary): void
}