import {IPage} from './document'
import {IRect} from './rect'

export interface IAnnotation {
    hasAppearanceStream(): boolean
    setBorderStyle(horizontalRadius:number, verticalRadius:number, width:number): void
    hasDestination():boolean
    hasAction():boolean
    getType():boolean
    setFileAttachment():void
    hasFileAttachment():boolean
}
export enum NPdfAnnotation {
    Text = 0,                   // - supported
    Link,                       // - supported
    FreeText,       // PDF 1.3  // - supported
    Line,           // PDF 1.3  // - supported
    Square,         // PDF 1.3
    Circle,         // PDF 1.3
    Polygon,        // PDF 1.5
    PolyLine,       // PDF 1.5
    Highlight,      // PDF 1.3
    Underline,      // PDF 1.3
    Squiggly,       // PDF 1.4
    StrikeOut,      // PDF 1.3
    Stamp,          // PDF 1.3
    Caret,          // PDF 1.5
    Ink,            // PDF 1.3
    Popup,          // PDF 1.3  // - supported
    FileAttachement,// PDF 1.3
    Sound,          // PDF 1.2
    Movie,          // PDF 1.2
    Widget,         // PDF 1.2  // - supported
    Screen,         // PDF 1.5
    PrinterMark,    // PDF 1.4
    TrapNet,        // PDF 1.3
    Watermark,      // PDF 1.6
    _3D,             // PDF 1.6
    RichMedia,      // PDF 1.7 ADBE ExtensionLevel 3 ALX: Petr P. Petrov
    WebMedia,       // PDF 1.7 IPDF ExtensionLevel 3
}

export enum NpdfAnnotationFlag {
    Invisible    = 0x0001,
    Hidden       = 0x0002,
    Print        = 0x0004,
    NoZoom       = 0x0008,
    NoRotate     = 0x0010,
    NoView       = 0x0020,
    ReadOnly     = 0x0040,
    Locked       = 0x0080,
    ToggleNoView = 0x0100,
    LockedContents = 0x0200
}

 export enum NpdfAction {
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
 }
export class Annotation implements IAnnotation {
    hasAppearanceStream(): boolean {
        throw new Error("Method not implemented.");
    }
    setBorderStyle(horizontalRadius: number, verticalRadius: number, width: number): void {
        throw new Error("Method not implemented.");
    }
    hasDestination(): boolean {
        throw new Error("Method not implemented.");
    }
    hasAction(): boolean {
        throw new Error("Method not implemented.");
    }
    getType(): boolean {
        throw new Error("Method not implemented.");
    }
    setFileAttachment(): void {
        throw new Error("Method not implemented.");
    }
    hasFileAttachment(): boolean {
        throw new Error("Method not implemented.");
    }
    constructor(page: IPage, type: NPdfAnnotation, rect: IRect) { }
}