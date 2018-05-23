import {IDocument} from "./document"
import {IDictionary, IObj} from "./object";
import {IPage} from "../dist/page";

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
export enum NPDFDestinationType {
    XYZ,
    Fit,
    FitH,
    FitV,
    FitR,
    FitB,
    FitBH,
    FitBV,
    Unknown = 0xFF
}
export interface IDestination {
    new(): IDestination
    readonly page: IPage
    readonly type: NPDFDestinationType

}
export interface IAction {
    new(type:NPDFActions, doc:IDocument): void
    type: NPDFActions
    uri?: string
    script?: string

    getObject(): IObj
    addToDictionary(dictionary: IDictionary): void
}