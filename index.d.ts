/// <reference types="node" />

export const CONVERSION = 0.0028346456693
export type NPDFExternal<T> = Object
export type Callback<T> = (err: Error, data: T) => void

export function pdfDate(d: Date): string

export interface NPDFFontMetrics {
    lineSpacing: number
    underlineThickness: number
    underlinePosition: number
    strikeOutThickness: number
    strikeOutPosition: number
    fileName: string
    fontData: string
    fontWeight: number
    fontSize: number
    fontScale: number
    charSpace: number
    wordSpace: number
    fontType?: string
}

export type NPDFcmyk = [number, number, number, number]
export type NPDFrgb = [number, number, number]
export type NPDFGrayScale = number
export type NPDFPoint = { x: number, y: number }

export enum NPDFStokeStyle {
    Solid,
    Dash,
    Dot,
    DashDot,
    DashDotDot
}

export enum NPDFHighlightingMode {
    None,
    Invert,
    InvertOutline,
    Push,
    Unknown = 0xff
}

export enum NPDFFontType {
    TrueType,
    Type1Pfa,
    Type1Pfb,
    Type1Base14,
    Type3
}

export enum NPDFColorSpace {
    DeviceGray,
    DeviceRGB,
    DeviceCMYK,
    Separation,
    CieLab,
    Indexed
}

export enum NPDFTextRenderingMode {
    Fill,
    Stroke,
    FillAndStroke,
    Invisible,
    FillToClipPath,
    StrokeToClipPath,
    FillAndStrokeToClipPath,
    ToClipPath
}

export enum NPDFLineCapStyle {
    Butt,
    Round,
    Square
}

export enum NPDFLineJoinStyle {
    Miter,
    Round,
    Bevel
}

/**
 * Top - Align with the top of the containing Rect
 * Center - Aligns Center, uses the set font's font metrics for calculating center
 * Bottom - Aligns with the bottom of the containing Rect
 */
export enum NPDFVerticalAlignment {
    Top,
    Center,
    Bottom
}

/**
 * Left - Does nothing, this is the default behaviour
 * Center - Calculates center using font's font metrics stringWidth operation ( / 2)
 * Right - Calculates center using font's font metrics stringWidth operation
 */
export enum NPDFAlignment {
    Left,
    Center,
    Bottom
}

export type NPDFDictionaryKeyType = 'boolean' | 'long' | 'name' | 'real'

export type NPDFInternal = any

export type NPDFCoerceKeyType = 'boolean' | 'long' | 'name' | 'real'

export type NPDFDataType = 'Boolean' | 'Number' | 'Name' | 'Real' | 'String' | 'Array' |
    'Dictionary' | 'Reference' | 'RawData'

export enum NPDFSigFlags {
    SignatureExists = 1,
    AppendOnly = 2,
    SignatureExistsAppendOnly = 3
}

export enum NPDFImageFormat {
    data,
    png,
    tiff,
    jpeg,
}

export enum NPDFFontEncoding {
    WinAnsi = 1,
    Standard = 2,
    PdfDoc = 3,
    MacRoman = 4,
    MacExpert = 5,
    Symbol = 6,
    ZapfDingbats = 7,
    Win1250 = 8,
    Iso88592 = 9,
    Identity = 0
}

export enum NPDFPageMode {
    DontCare,
    UseNone,
    UseThumbs,
    UseBookmarks,
    FullScreen,
    UseOC,
    UseAttachments
}

export enum NPDFPageLayout {
    Ignore,
    Default,
    SinglePage,
    OneColumn,
    TwoColumnLeft,
    TwoColumnRight,
    TwoPageLeft,
    TwoPageRight
}

export interface NPDFCreateFontOpts {
    fontName: string,
    bold?: boolean,
    italic?: boolean,
    encoding?: NPDFFontEncoding,
    embed?: boolean,
    fileName?: string
}

export enum NPDFAnnotationAppearance {
    normal,
    rollover,
    down
}

export type ListItem = {
    value: string,
    display: string
}

export interface NPDFTextFieldOpts {
    maxLen?: number
    multiLine?: boolean
    passwordField?: boolean
    fileField?: boolean
    spellCheckEnabled?: boolean
    scrollEnabled?: boolean
    /**
     * @desc Divide text into equal length combs.
     * @requires NPDFTextFieldOpts#maxLen
     */
    combs?: boolean
    richText?: boolean
}

export enum NPDFCertificatePermission {
    NoPerms = 1,
    FormFill = 2,
    Annotations = 3,
}

export enum NPDFFieldType {
    PushButton,
    Checkbox,
    RadioButton,
    TextField,
    ComboBox,
    ListBox,
    Signature,
    Unknown = 0xff
}

export type NPDFAnnotationBorderStyle = { horizontal: number, vertical: number, width: number }

export type ProtectionOption =
    'Copy'
    | 'Print'
    | 'Edit'
    | 'EditNotes'
    | 'FillAndSign'
    | 'Accessible'
    | 'DocAssembly'
    | 'HighPrint'

export type EncryptOption = {
    userPassword?: string
    ownerPassword?: string
    protection?: Array<ProtectionOption>
    algorithm?: 'rc4v1' | 'rc4v2' | 'aesv2' | 'aesv3'
    keyLength?: number
}

export type ProtectionSummary = {
    Accessible: boolean
    Print: boolean
    Copy: boolean
    DocAssembly: boolean
    Edit: boolean
    EditNotes: boolean
    FillAndSign: boolean
    HighPrint: boolean
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

export enum NPDFDestinationFit {
    Fit,
    FitH,
    FitV,
    FitB,
    FitBH,
    FitBV,
    Unknown = 0xFF
}

export enum NPDFAnnotation {
    Text = 0,
    Link,
    FreeText,
    Line,
    Square,
    Circle,
    Polygon,
    PolyLine,
    Highlight,
    Underline,
    Squiggly,
    StrikeOut,
    Stamp,
    Caret,
    Ink,
    Popup,
    //FileAttachement, // To create a file attachment use Document.attachFile() method
    Sound = 17,
    Movie,
    Widget,
    Screen,
    PrinterMark,
    TrapNet,
    Watermark,
    _3D,
    RichMedia,
    WebMedia,
}

export enum NPDFAnnotationFlag {
    Invisible = 0x0001,
    Hidden = 0x0002,
    Print = 0x0004,
    NoZoom = 0x0008,
    NoRotate = 0x0010,
    NoView = 0x0020,
    ReadOnly = 0x0040,
    Locked = 0x0080,
    ToggleNoView = 0x0100,
    LockedContents = 0x0200
}

export enum NPDFAnnotationType {
    Text = 'Text',
    Link = 'Link',
    FreeText = 'FreeText',
    Line = 'Line',
    Square = 'Square',
    Circle = 'Circle',
    Polygon = 'Polygon',
    PolyLine = 'PolyLine',
    Highlight = 'Highlight',
    Underline = 'Underline',
    Squiggly = 'Squiggly',
    StrikeOut = 'StrikeOut',
    Stamp = 'Stamp',
    Caret = 'Caret',
    Ink = 'Ink',
    Popup = 'Popup',
    FileAttachment = 'FileAttachment',
    Sound = 'Sound',
    Movie = 'Movie',
    Widget = 'Widget',
    Screen = 'Screen',
    PrinterMark = 'PrinterMark',
    TrapNet = 'TrapNet',
    Watermark = 'Watermark',
    _3D = '3D',
    RichMedia = 'RichMedia',
    WebMedia = 'WebMedia'
}

export interface NPDFInfo {
    author: string
    createdAt: Date
    creator: string
    keywords: string
    producer: string
    subject: string
    title: string
}

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

export enum NPDFBlendMode {
    Normal = "Normal",
    Multiply = "Multiply",
    Screen = "Screen",
    Overlay = "Overlay",
    Darken = "Darken",
    Lighten = "Lighten",
    ColorDodge = "ColorDodge",
    ColorBurn = "ColorBurn",
    HardLight = "HardLight",
    SoftLight = "SoftLight",
    Difference = "Difference",
    Exclusion = "Exclusion",
    Hue = "Hue",
    Saturation = "Saturation",
    Color = "Color",
    Luminosity = "Luminosity"
}

export enum NPDFRenderingIntent {
    AbsoluteColorimetric = "AbsoluteColorimetric",
    RelativeColorimetric = "RelativeColorimetric",
    Perceptual = "Perceptual",
    Saturation = "Saturation"
}

export namespace nopodofo {
    /**
     * An Action can be used in conjunction with an outline to create bookmarks on the pdf
     * Action can also used to link annotatoins to external sources, as well as run scripts.
     * @see Outline
     * @see Annotation
     */
    export class Action {
        constructor(doc: Base, type: NPDFActions)

        readonly type: NPDFActions
        uri?: string
        script?: string

        getObject(): Object

        addToDictionary(dictionary: Dictionary): void
    }

    /**
     * Annotations are the core of all pdf widgets, this includes AcroForm fields, sticky notes, links, etc...
     * An Annotation is created via document page.createAnnotation. There is no other way to create a new annotation.
     * Annotations are a lower level PDF object, please familiarize yourself with the PDF Spec for usage.
     * {@link https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/PDF32000_2008.pdf}
     * @see Page
     */
    export class Annotation {
        flags: NPDFAnnotationFlag
        title: string
        content: string
        destination: Destination
        action: Action
        open: boolean
        quadPoints: number[]
        color: Color
        attachment: FileSpec
        rect: Rect

        setBorderStyle(v: { horizontal: number, vertical: number, width: number }, stroke?: nopodofo.Array): void

        hasAppearanceStream(): boolean

        setAppearanceStream(xobj: XObject): void

        getType(): NPDFAnnotationType
    }

    export class Data {
        constructor(value: string | Buffer)

        readonly value: string

        write(output: string): void
    }

    /**
     * The PdfDate object is a specific format for Date objects in Pdf
     */
    export class Date {
        /**
         * Create a new PdfDate object with a timestamp of now
         * @returns {Date}
         */
        constructor()

        /**
         * Create an existing timestamp from a string.
         * The string must be formatted in the following:
         * (D:YYYYMMDDHHmmSSOHH'mm')
         * @param {string} timestamp
         * @returns {Date}
         */
        constructor(timestamp: string)

        /**
         * returns in the the following format: (D:YYYYMMDDHHmmSSOHH'mm')
         * @returns {string}
         */
        toString(): string
    }

    export class Destination {
        constructor(page: Page, fit: NPDFDestinationFit)
        constructor(page: Page, fit: NPDFDestinationFit, fitArg: number)
        constructor(page: Page, left: number, top: number, zoom: number)

        readonly page: Page
        readonly type: NPDFDestinationType
    }

    export class Encrypt {
        static createEncrypt(opts: EncryptOption): NPDFExternal<Encrypt>

        constructor(doc: Document)

        owner: string
        user: string
        encryptionKey: string
        keyLength: number
        protections: ProtectionSummary

        isAllowed(action: ProtectionOption): boolean
    }

    abstract class Field {
        readOnly: boolean
        required: boolean
        fieldName: string
        alternateName?: string
        mappingName?: string
        exported?: boolean
        AP?: Dictionary
        DA?: string | null
        readonly widgetAnnotation: Annotation
        readonly type: NPDFFieldType
        readonly obj: nopodofo.Object

        setBackgroundColor(color: Color): void

        setBorderColor(color: Color): void

        setHighlightingMode(mode: NPDFHighlightingMode): void

        setMouseAction(on: NPDFMouseEvent, action: Action): void

        setDate(dateTime?: string | nopodofo.Date): void

        setPageAction(on: NPDFPageEvent, action: Action): void
    }

    export class Color {
        constructor()
        constructor(grey: number)
        constructor(fromString: string)
        constructor(red: number, green: number, blue: number)
        constructor(cyan: number, magenta: number, yellow: number, black: number)

        /**
         * Get the stream representation of this color
         */
        getColorStreamString(): string

        isRGB(): boolean

        isCMYK(): boolean

        isGreyScale(): boolean

        convertToGreyScale(): Color

        convertToCMYK(): Color

        convertToRGB(): Color

        getGrey(): number

        getRed(): number

        getGreen(): number

        getBlue(): number

        getCyan(): number

        getMagenta(): number

        getYellow(): number

        getBlack(): number
    }

    export class TextField extends Field {
        /**
         * @desc Create from an existing Field
         */
        constructor(page: Page, fieldIndex: number)

        /**
         * @desc Creates a new TextField
         */
        constructor(page: Page, annotation: Annotation, form: Form, opts?: Object)

        text: string
        maxLen: number
        multiLine: boolean
        passwordField: boolean
        fileField: boolean
        spellCheckEnabled: boolean
        scrollEnabled: boolean
        combs: boolean
        richText: boolean
        alignment?: NPDFAlignment

        refreshAppearanceStream(): void
    }

    export class Checkbox extends Field {
        /**
         * @desc Create from an existing Field
         */
        constructor(page: Page, fieldIndex: number)

        /**
         * @desc Creates a new TextField
         */
        constructor(form: Form, annotation: Annotation)

        checked: boolean
    }

    abstract class ListField extends Field {
        selected: number
        length: number
        spellCheckEnabled: boolean
        sorted: boolean
        multiSelect: boolean

        isComboBox(): boolean

        insertItem(value: string, displayName: string): void

        removeItem(index: number): void

        getItem(index: number): ListItem
    }

    export class ComboBox extends ListField {
        /**
         * @desc Create from an existing Field
         */
        constructor(page: Page, fieldIndex: number)

        /**
         * @desc Creates a new TextField
         */
        constructor(form: Form, annotation: Annotation)

        editable: boolean
    }

    export class ListBox extends ListField {
        /**
         * @desc Create from an existing Field
         */
        constructor(page: Page, fieldIndex: number)

        /**
         * @desc Creates a new TextField
         */
        constructor(form: Form, annotation: Annotation)

    }

    export class PushButton extends Field {
        /**
         * @desc Create from an existing Field
         */
        constructor(page: Page, fieldIndex: number)

        /**
         * @desc Creates a new TextField
         */
        constructor(form: Form, annotation: Annotation)

        rollover: string
        rolloverAlternate: string
    }

    export class SignatureField {
        readonly widgetAnnotation: Annotation
        readonly obj: Object

        constructor(annotation: Annotation, doc: Document)

        setAppearanceStream(xObj: any, appearance: NPDFAnnotationAppearance, name: string): void

        setReason(reason: string): void

        setLocation(location: string): void

        setCreator(creator: string): void

        setDate(dateTime?: string): void

        addCertificateReference(perm: NPDFCertificatePermission): void

        setFieldName(name: string): void

        getObject(): Object

        ensureSignatureObject(): void

        getSignatureInfo(): {}
    }

    export class FileSpec {
        /**
         * Create a new FileSpec object
         * @param file
         * @param doc
         * @param embed
         */
        constructor(file: string, doc: Base, embed?: boolean)

        /**
         * Copy an existing FileSpec from an Obj
         */
        constructor(obj: Object)

        readonly name: string

        /**
         * @desc Can only get the file if it has been embedded into the document.
         * @returns {Buffer | undefined}
         */
        getContents(): Buffer | undefined
    }

    export class Form {
        needAppearances: boolean
        dictionary: Dictionary
        DA?: string
        DR?: Dictionary
        CO?: Dictionary
        SigFlags?: NPDFSigFlags

        // createAppearanceStream<T extends Field>(bg: Color, fg: Color, font: Font, size: number)
    }

    export class Image {
        /**
         *
         * @param {Base} doc
         * @param {string | Buffer} source
         * @param {NPDFImageFormat} [format] - defaults to data
         * @returns {Image}
         */
        constructor(doc: Base, source: string | Buffer, format?: NPDFImageFormat)

        readonly width: number
        readonly height: number

        setInterpolate(v: boolean): void
    }

    /**
     * @desc Document represents a PdfMemDocument, construct from an existing pdf document.
     * Document is the core class for reading and manipulating PDF files and writing them back to disk.
     * Document was designed to allow easy access to the object structure of a PDF file.
     * Document should be used whenever you want to change the object structure of a PDF file.
     */
    export class Document extends Base {
        constructor()

        encrypt: Encrypt
        readonly trailer: Object
        readonly catalog: Object

        load(file: string | Buffer,
             opts: {
                 forUpdate?: boolean,
                 password?: string
             },
             cb: Callback<void>): void
        load(file: string | Buffer, cb: Callback<Document>): void

        setPassword(pwd: string): void

        /**
         * Find a font in the Documents font cache by name or id
         * @param name
         */
        getFont(name: string): Font

        /**
         * Get a list of font {name, id} from the current document
         */
        listFonts(): {name:string, id: string, file: string}[]

        /**
         * Deletes one or more pages from the document by removing the pages reference
         * from the pages tree. This does NOT remove the page object as the page object
         * may be used by other objects in the document.
         * @param startIndex - first page to delete (0-based)
         * @param count - number of pages to delete
         */
        splicePages(startIndex: number, count: number): void

        /**
         * Copies one or more pages from another pdf to this document
         * This function copies the entire document to the target document and then
         * deletes pages that are not of interest. This is a much faster process, but
         * without object garbage collection the document may result in a much larger
         * than necessary document
         * @see {Document#gc}
         * @param fromDoc - PdfMemDocument to append
         * @param startIndex - first page to copy (0-based)
         * @param count - number of pages to copy
         */
        insertPages(fromDoc: Document, startIndex: number, count: number): number

        /**
         * Persist the document with any changes applied to either a new nodejs buffer or to disk.
         * @param destination - file path or callback function
         * @param cb - if file path was provided as destination, this must be a callback function
         */
        write(destination: Callback<Buffer> | string, cb?: Callback<string>): void

        /**
         * Performs garbage collection on the document. All objects not
         * reachable by the trailer are deleted.
         * @param file - pdf document, file path or node buffer
         * @param pwd - if document is password protected this parameter is required
         * @param cb - function
         */
        static gc(file: Buffer, pwd: string, cb: Callback<Buffer>): void
        static gc(file: Buffer, cb: Callback<Buffer>): void

        /**
         * Check for the existence of a signature field(s)
         * @returns {boolean}
         */
        hasSignatures(): boolean

        /**
         * Iterate each page annotations array for an annotation of type signature.
         * @returns {SignatureField[]}
         */
        getSignatures(): SignatureField[]

        append(doc: Document | Document[]): void

        insertExistingPage(memDoc: Document, index: number, insertIndex: number): number

    }

    abstract class Base {
        readonly form: Form
        readonly body: Object[]
        readonly version: NPDFVersion
        pageMode: NPDFPageMode
        pageLayout: NPDFPageLayout
        printingScale: string
        baseUri: string
        language: string
        readonly info: NPDFInfo

        getPageCount(): number

        getPage(n: number): Page

        hideToolbar(): void

        hideMenubar(): void

        hideWindowUI(): void

        fitWindow(): void

        centerWindow(): void

        displayDocTitle(): void

        useFullScreen(): void

        attachFile(file: string): void

        insertPage(rect: Rect, index: number): Page

        isLinearized(): boolean

        getWriteMode(): NPDFWriteMode

        isAllowed(perm: ProtectionOption): boolean

        createFont(opts: NPDFCreateFontOpts): Font

        createFontSubset(opts: NPDFCreateFontOpts): Font

        /**
         * Get an existing outline or create a new outline and new root node
         * @param {boolean} [create] - Create a new outline if one does not already exist
         * @param {string} [root] - Create a Root node with the provided name
         */
        getOutlines(create?: boolean, root?: string): null | Outline

        getObject(ref: Ref): Object

        getNames(create: boolean): Object | null

        createXObject(rect: Rect): XObject

        createPage(rect: Rect): Page

        createPages(rects: Rect[]): number

        getAttachment(fileName: string): FileSpec

        addNamedDestination(page: Page, destination: NPDFDestinationFit, name: string): void
    }

    export class Object {
        readonly reference: Ref
        readonly length: number
        readonly stream: Stream
        readonly type: NPDFDataType
        immutable: boolean

        constructor()
        constructor(s: string)
        constructor(a: string[] | number[])
        constructor(d: number)

        hasStream(): boolean

        getOffset(key: string): Promise<number>

        write(output: string, cb: Function): void

        flateCompressStream(): void

        delayedStreamLoad(): void

        getBool(): boolean

        getDictionary(): Dictionary

        getString(): string

        getName(): string

        getReal(): number

        getNumber(): number

        getArray(): Array

        /**
         * Copies the raw data to a nodejs buffer.
         */
        getRawData(): Buffer

        clear(): void

        resolveIndirectKey(key: string): Object
    }

    export class Array {
        dirty: boolean
        readonly length: number
        immutable: boolean

        constructor()

        /**
         * If the item at the index is a Reference that can not be resolved by the array object owner.
         * A Ref will be returned, the Ref can be resolved using getObject on the document itself.
         * @todo Fix null owner when trying to resolve Reference type
         * @param i
         */
        at(i: number): Ref | Object

        pop(): Object

        clear(): void

        push(v: Object): void

        write(destination: string): void
    }

    export class Ref {
        readonly gennum: number
        readonly objnum: number
    }

    export class Dictionary {
        dirty: boolean
        immutable: boolean
        readonly obj: Object

        constructor()

        getKeyType(k: string): NPDFDataType

        /**
         * @param k - dictionary key
         * @param resolveType - if true the value returned will be the type defined by Obj.type, otherwise an Obj is returned
         */
        getKey<T>(k: string, resolveType?: boolean): T

        addKey(prop: string, value: boolean | number | string | Object | Ref | Dictionary): void

        getKeys(): string[]

        hasKey(k: string): boolean

        removeKey(k: string): void

        getKeyAs(k: string, t: NPDFDictionaryKeyType): string | number

        clear(): void

        write(destination: string, cb: (e: Error, i: string) => void): void

        writeSync(destination: string): void
    }

    /**
     * This class is a parser for content streams in PDF documents.
     * PoDoFo::PdfContentsTokenizer is currently a work in progress.
     */
    export class ContentsTokenizer {
        constructor(doc: Base, pageIndex: number)

        readSync(): Iterator<string>

        read(cb: Callback<string>): void
    }

    export class XObject {
        readonly contents: Object
        readonly contentsForAppending: Object
        readonly resources: Object
        readonly reference: Ref
        readonly pageMediaBox: Rect

        /**
         * Use Base.createXObject instance method for creating new instance's and copy instance's of an XObject.
         */
        constructor(native: NPDFExternal<XObject>)
        /**
         * Use Base.createXObject instance method for creating new instance's and copy instance's of an XObject.
         */
        constructor(rect: Rect, native: NPDFExternal<Base>)
    }


    export class StreamDocument extends Base {
        /**
         *
         * @param {string} [file]
         * @param {{version: NPDFVersion, writer: NPDFWriteMode, encrypt: Encrypt}} [opts] -
         *      defaults to {pdf1.7, writeMode_default, null}
         * @returns {StreamDocument}
         */
        constructor(file?: string, opts?: { version: NPDFVersion, writer: NPDFWriteMode, encrypt?: Encrypt })

        /**
         * Closing a stream document will prevent any further writes to the document.
         * If the object was instantiated with a file path, close will write to this file.
         * If "new" was called without any args, the document is written to a nodejs buffer, close
         *   will return the buffer.
         */
        close(): string | Buffer
    }

    export class Signer {
        signatureField: SignatureField

        /**
         * Creates a new instance of Signer. Signer is the only way to
         * @param {Document} doc
         * @param {string} [output] - optional if provided writes to this path, otherwise
         *      a buffer is returned
         * @returns {Signer}
         */
        constructor(doc: Document, output?: string)

        /**
         * Loads the Certificate and Private Key and stores the values into the Signer instance.
         * Values are not retrievable but are stored for use in Signer.sign
         * After both cert and pkey are loaded, a minimal signature size is calculated and returned to
         * the caller. To complete the signing process this minimum signature size value needs to be
         * provided to the write method.
         *
         * @param {string} certificate
         * @param {string} pkey
         * @param {string | Callback} p - either the pkey password or callback
         * @param {Callback} [cb] - callback
         * @returns {Number} - minimum signature size
         */
        loadCertificateAndKey(certificate: string, pkey: string, p: string | Callback<Number>, cb?: Callback<Number>): number

        /**
         * Signs the document output to disk or a node buffer
         * The loadCertificateAndKey must be loaded prior to calling write
         * @see loadCertificateAndKey
         * @param {Number} minSignatureSize
         * @param {Callback} cb
         */
        write(minSignatureSize: Number, cb: Callback<Buffer | string>): void
    }


    export class Rect {
        /**
         * Create a new PdfRect with values set to zero.
         */
        constructor()

        /**
         * Create a new PdfRect with values provided
         * @param left
         * @param bottom
         * @param width
         * @param height
         */
        constructor(left: number, bottom: number, width: number, height: number)

        left: number
        bottom: number
        width: number
        height: number

        intersect(rect: Rect): void
    }

    export class Painter {
        tabWidth: number
        readonly canvas: Stream
        font?: Font
        precision: number

        constructor(doc: Base)

        setPage(page: Page | XObject): void

        setColor(rgb: Color): void

        setStrokeWidth(w: number): void

        setGrey(v: number): void

        setStrokingGrey(v: number): void

        setColorCMYK(cmyk: Color): void

        setStrokingColorCMYK(cmyk: Color): void

        setStrokeStyle(style: NPDFStokeStyle): void

        setLineCapStyle(style: NPDFLineCapStyle): void

        setLineJoinStyle(style: NPDFLineJoinStyle): void

        setClipRect(rect: Rect): void

        setMiterLimit(v: number): void

        rectangle(rect: Rect): void

        ellipse(points: NPDFPoint & { width: number, height: number }): void

        circle(points: NPDFPoint & { radius: number }): void

        closePath(): void

        lineTo(point: NPDFPoint): void

        moveTo(point: NPDFPoint): void

        cubicBezierTo(p1: NPDFPoint, p2: NPDFPoint, p3: NPDFPoint): void

        horizontalLineTo(v: number): void

        verticalLineTo(v: number): void

        smoothCurveTo(p1: NPDFPoint, p2: NPDFPoint): void

        quadCurveTo(p1: NPDFPoint, p2: NPDFPoint): void

        arcTo(p1: NPDFPoint, p2: NPDFPoint, rotation: number, large?: boolean, sweep?: boolean): void

        close(): void

        stroke(): void

        fill(): void

        strokeAndFill(): void

        endPath(): void

        clip(): void

        save(): void

        restore(): void

        setExtGState(state: ExtGState): void

        getCurrentPath(): string

        drawLine(p1: NPDFPoint, p2: NPDFPoint): void

        drawText(point: NPDFPoint, text: string): void

        drawTextAligned(point: NPDFPoint & { width: number }, text: string, alignment: NPDFAlignment): void

        drawMultiLineText(rect: Rect, value: string, alignment?: NPDFAlignment, vertical?: NPDFVerticalAlignment): void

        getMultiLineText(width: number, text: string, skipSpaces?: boolean): Array

        beginText(point: NPDFPoint): void

        endText(): void

        addText(text: string): void

        moveTextPosition(point: NPDFPoint): void

        drawGlyph(point: NPDFPoint, glyph: string): void

        finishPage(): void

        /**
         *
         * @param {Image} img - an instance of Image
         * @param {number} x - x coordinate (bottom left position of image)
         * @param {number} y - y coordinate (bottom position of image)
         * @param {{width:number, heigth:number}} scale - optional scaling
         */
        drawImage(img: Image, x: number, y: number, scale?: { width: number, height: number }): void

    }

    export class Font {
        object: Object
        size: number
        scale: number
        charSpace: number
        wordSpace: number
        underline: boolean
        strikeOut: boolean
        identifier: string

        isBold(): boolean

        isItalic(): boolean

        getEncoding(): Encoding

        getMetrics(): NPDFFontMetrics

        stringWidth(v: string): number

        write(content: string, stream: Stream): void

        embed(): void

        isSubsetting(): boolean

        embedSubsetFont(): void
    }

    export class Encoding {
        addToDictionary(target: Object): void

        convertToUnicode(content: string, font: Font): string

        convertToEncoding(content: string, font: Font): Buffer
    }

    export class ExtGState {
        constructor()

        setFillOpacity(v: number): void

        setBlendMode(mode: NPDFBlendMode): void

        setOverprint(v: boolean): void

        setFillOverprint(v: boolean): void

        setStrokeOpacity(v: number): void

        setStrokeOverprint(v: boolean): void

        setNonZeroOverprint(v: boolean): void

        setRenderingIntent(intent: NPDFRenderingIntent): void

        setFrequency(v: number): void

    }

    export class Page {
        rotation: number
        trimBox: Rect
        number: number
        width: number
        height: number
        contents: Object
        resources: Object

        /**
         * Get the field as a field of type T where T is one of: Checkbox, Pushbutton, Textfield, ComboBox, or ListField
         * @param index - field index
         * @see Checkbox
         * @see ListBox
         * @see TextField
         * @see ComboBox
         * @see PushButton
         */
        getField<T extends Field>(index: number): T

        /**
         * Get all fields on this page.
         * Note: to determine the type of field use the Field.type property
         */
        getFields(): Field[]

        fieldCount(): number

        getFieldIndex(fieldName: string): number

        getMediaBox(): Rect

        getBleedBox(): Rect

        getArtBox(): Rect

        annotationCount(): number

        createAnnotation(type: NPDFAnnotation, rect: Rect): Annotation

        createField(type: NPDFFieldType, annot: Annotation, form: Form, opts?: Object): Field

        deleteField(index: number): void

        /**
         * Flattening is the process of taking a fields appearance stream, appending that appearance stream
         * to the page, and then removing the field object and annotation widget, and scrubbing all references
         * to the field from the document (scrub the page and acroform dictionary)
         */
        flattenFields(): void

        getAnnotation(index: number): Annotation

        deleteAnnotation(index: number): void
    }

    /**
     * Outline represents an PdfOutlineItem.
     * An Outline is created by another outline, or the outline root
     * @see Base#getOutlines
     */
    export class Outline {
        readonly prev: Outline | null
        readonly next: Outline | null
        readonly first: Outline | null
        readonly last: Outline | null
        destination: Destination
        action: Action
        title: string
        textFormat: number
        textColor: Color

        createChild(name: string, value: Destination): Outline

        createNext(name: string, value: Destination | Action): Outline

        insertItem(item: Object): void

        getParent(): Outline

        erase(): void
    }

    export class Stream {
        constructor(parent: nopodofo.Object)

        write(cb: Callback<Buffer>): void
        write(data: string, cb: Callback<string>): void

        beginAppend(): void

        append(data: string | Buffer): void

        endAppend(): void

        inAppendMode(): boolean

        set(data: string | Buffer): void

        /**
         * Copy creates and returns a copy of `this.data`
         * @param filtered - Get the copy as a filtered or unfiltered copy.
         */
        copy(filtered?: boolean): Buffer
    }

    export class SimpleTable {
        constructor(doc: Base, cols: number, rows: number)

        borderWidth: number
        foregroundColor: Color
        backgroundColor: Color
        alignment: string
        wordWrap: boolean
        tableWidth: number
        tableHeight: number
        autoPageBreak: boolean

        getText(col: number, row: number): string

        setText(col: number, row: number, text: string): void

        getFont(col: number, row: number): Font

        setFont(font: Font): void

        getVerticalAlignment(col: number, row: number): string

        borderEnable(v: boolean): void

        hasBorders(): boolean

        getImage(col: number, row: number): Buffer

        hasImage(col: number, row: number): boolean

        hasBackgroundColor(col: number, row: number): boolean

        enableBackground(v: boolean): void

        getBorderColor(col: number, row: number): Color

        draw(point: NPDFPoint, painter: Painter): void

        columnCount(): number

        rowCount(): number

        setColumnWidths(n: number[]): void

        setColumnWidth(n: number): void

        setRowHeight(n: number): void

        setRowHeights(n: number[]): void
    }
}

export enum NPDFName {
    A = "A",
    AA = "AA",
    AC = "AC",
    ACRO_FORM = "AcroForm",
    ACTUAL_TEXT = "ActualText",
    ADBE_PKCS7_DETACHED = "adbe.pkcs7.detached",
    ADBE_PKCS7_SHA1 = "adbe.pkcs7.sha1",
    ADBE_X509_RSA_SHA1 = "adbe.x509.rsa_sha1",
    ADOBE_PPKLITE = "Adobe.PPKLite",
    AESV2 = "AESV2",
    AESV3 = "AESV3",
    AFTER = "After",
    AIS = "AIS",
    ALT = "Alt",
    ALPHA = "Alpha",
    ALTERNATE = "Alternate",
    ANNOT = "Annot",
    ANNOTS = "Annots",
    ANTI_ALIAS = "AntiAlias",
    AP = "AP",
    AP_REF = "APRef",
    APP = "App",
    ART_BOX = "ArtBox",
    ARTIFACT = "Artifact",
    AS = "AS",
    ASCENT = "Ascent",
    ASCII_HEX_DECODE = "ASCIIHexDecode",
    ASCII_HEX_DECODE_ABBREVIATION = "AHx",
    ASCII85_DECODE = "ASCII85Decode",
    ASCII85_DECODE_ABBREVIATION = "A85",
    ATTACHED = "Attached",
    AUTHOR = "Author",
    AVG_WIDTH = "AvgWidth",
    B = "B",
    BACKGROUND = "Background",
    BASE_ENCODING = "BaseEncoding",
    BASE_FONT = "BaseFont",
    BASE_STATE = "BaseState",
    BBOX = "BBox",
    BC = "BC",
    BE = "BE",
    BEFORE = "Before",
    BG = "BG",
    BITS_PER_COMPONENT = "BitsPerComponent",
    BITS_PER_COORDINATE = "BitsPerCoordinate",
    BITS_PER_FLAG = "BitsPerFlag",
    BITS_PER_SAMPLE = "BitsPerSample",
    BLACK_IS_1 = "BlackIs1",
    BLACK_POINT = "BlackPoint",
    BLEED_BOX = "BleedBox",
    BM = "BM",
    BORDER = "Border",
    BOUNDS = "Bounds",
    BPC = "BPC",
    BS = "BS",
    BTN = "Btn",
    BYTERANGE = "ByteRange",
    C = "C",
    C0 = "C0",
    C1 = "C1",
    CA = "CA",
    CA_NS = "ca",
    CALGRAY = "CalGray",
    CALRGB = "CalRGB",
    CAP = "Cap",
    CAP_HEIGHT = "CapHeight",
    CATALOG = "Catalog",
    CCITTFAX_DECODE = "CCITTFaxDecode",
    CCITTFAX_DECODE_ABBREVIATION = "CCF",
    CENTER_WINDOW = "CenterWindow",
    CERT = "Cert",
    CF = "CF",
    CFM = "CFM",
    CH = "Ch",
    CHAR_PROCS = "CharProcs",
    CHAR_SET = "CharSet",
    CICI_SIGNIT = "CICI.SignIt",
    CID_FONT_TYPE0 = "CIDFontType0",
    CID_FONT_TYPE2 = "CIDFontType2",
    CID_TO_GID_MAP = "CIDToGIDMap",
    CID_SET = "CIDSet",
    CIDSYSTEMINFO = "CIDSystemInfo",
    CL = "CL",
    CLR_F = "ClrF",
    CLR_FF = "ClrFf",
    CMAP = "CMap",
    CMAPNAME = "CMapName",
    CMYK = "CMYK",
    CO = "CO",
    COLOR = "Color",
    COLOR_BURN = "ColorBurn",
    COLOR_DODGE = "ColorDodge",
    COLORANTS = "Colorants",
    COLORS = "Colors",
    COLORSPACE = "ColorSpace",
    COLUMNS = "Columns",
    COMPATIBLE = "Compatible",
    COMPONENTS = "Components",
    CONTACT_INFO = "ContactInfo",
    CONTENTS = "Contents",
    COORDS = "Coords",
    COUNT = "Count",
    CP = "CP",
    CREATION_DATE = "CreationDate",
    CREATOR = "Creator",
    CROP_BOX = "CropBox",
    CRYPT = "Crypt",
    CS = "CS",
    D = "D",
    DA = "DA",
    DARKEN = "Darken",
    DATE = "Date",
    DCT_DECODE = "DCTDecode",
    DCT_DECODE_ABBREVIATION = "DCT",
    DECODE = "Decode",
    DECODE_PARMS = "DecodeParms",
    DEFAULT = "default",
    DEFAULT_CMYK = "DefaultCMYK",
    DEFAULT_GRAY = "DefaultGray",
    DEFAULT_RGB = "DefaultRGB",
    DESC = "Desc",
    DESCENDANT_FONTS = "DescendantFonts",
    DESCENT = "Descent",
    DEST = "Dest",
    DEST_OUTPUT_PROFILE = "DestOutputProfile",
    DESTS = "Dests",
    DEVICECMYK = "DeviceCMYK",
    DEVICEGRAY = "DeviceGray",
    DEVICEN = "DeviceN",
    DEVICERGB = "DeviceRGB",
    DI = "Di",
    DIFFERENCE = "Difference",
    DIFFERENCES = "Differences",
    DIGEST_METHOD = "DigestMethod",
    DIGEST_RIPEMD160 = "RIPEMD160",
    DIGEST_SHA1 = "SHA1",
    DIGEST_SHA256 = "SHA256",
    DIGEST_SHA384 = "SHA384",
    DIGEST_SHA512 = "SHA512",
    DIRECTION = "Direction",
    DISPLAY_DOC_TITLE = "DisplayDocTitle",
    DL = "DL",
    DM = "Dm",
    DOC = "Doc",
    DOC_CHECKSUM = "DocChecksum",
    DOC_TIME_STAMP = "DocTimeStamp",
    DOCMDP = "DocMDP",
    DOCUMENT = "Document",
    DOMAIN = "Domain",
    DOS = "DOS",
    DP = "DP",
    DR = "DR",
    DS = "DS",
    DUPLEX = "Duplex",
    DUR = "Dur",
    DV = "DV",
    DW = "DW",
    DW2 = "DW2",
    E = "E",
    EARLY_CHANGE = "EarlyChange",
    EF = "EF",
    EMBEDDED_FDFS = "EmbeddedFDFs",
    EMBEDDED_FILES = "EmbeddedFiles",
    EMPTY = "",
    ENCODE = "Encode",
    ENCODED_BYTE_ALIGN = "EncodedByteAlign",
    ENCODING = "Encoding",
    ENCODING_90MS_RKSJ_H = "90ms-RKSJ-H",
    ENCODING_90MS_RKSJ_V = "90ms-RKSJ-V",
    ENCODING_ETEN_B5_H = "ETen-B5-H",
    ENCODING_ETEN_B5_V = "ETen-B5-V",
    ENCRYPT = "Encrypt",
    ENCRYPT_META_DATA = "EncryptMetadata",
    END_OF_LINE = "EndOfLine",
    ENTRUST_PPKEF = "Entrust.PPKEF",
    EXCLUSION = "Exclusion",
    EXT_G_STATE = "ExtGState",
    EXTEND = "Extend",
    EXTENDS = "Extends",
    F = "F",
    F_DECODE_PARMS = "FDecodeParms",
    F_FILTER = "FFilter",
    FB = "FB",
    FDF = "FDF",
    FF = "Ff",
    FIELDS = "Fields",
    FILESPEC = "Filespec",
    FILTER = "Filter",
    FIRST = "First",
    FIRST_CHAR = "FirstChar",
    FIT_WINDOW = "FitWindow",
    FL = "FL",
    FLAGS = "Flags",
    FLATE_DECODE = "FlateDecode",
    FLATE_DECODE_ABBREVIATION = "Fl",
    FONT = "Font",
    FONT_BBOX = "FontBBox",
    FONT_DESC = "FontDescriptor",
    FONT_FAMILY = "FontFamily",
    FONT_FILE = "FontFile",
    FONT_FILE2 = "FontFile2",
    FONT_FILE3 = "FontFile3",
    FONT_MATRIX = "FontMatrix",
    FONT_NAME = "FontName",
    FONT_STRETCH = "FontStretch",
    FONT_WEIGHT = "FontWeight",
    FORM = "Form",
    FORMTYPE = "FormType",
    FRM = "FRM",
    FT = "FT",
    FUNCTION = "Function",
    FUNCTION_TYPE = "FunctionType",
    FUNCTIONS = "Functions",
    G = "G",
    GAMMA = "Gamma",
    GROUP = "Group",
    GTS_PDFA1 = "GTS_PDFA1",
    H = "H",
    HARD_LIGHT = "HardLight",
    HEIGHT = "Height",
    HIDE_MENUBAR = "HideMenubar",
    HIDE_TOOLBAR = "HideToolbar",
    HIDE_WINDOWUI = "HideWindowUI",
    HUE = "Hue",
    I = "I",
    IC = "IC",
    ICCBASED = "ICCBased",
    ID = "ID",
    ID_TREE = "IDTree",
    IDENTITY = "Identity",
    IDENTITY_H = "Identity-H",
    IDENTITY_V = "Identity-V",
    IF = "IF",
    IM = "IM",
    IMAGE = "Image",
    IMAGE_MASK = "ImageMask",
    INDEX = "Index",
    INDEXED = "Indexed",
    INFO = "Info",
    INKLIST = "InkList",
    INTERPOLATE = "Interpolate",
    IT = "IT",
    ITALIC_ANGLE = "ItalicAngle",
    ISSUER = "Issuer",
    IX = "IX",
    JAVA_SCRIPT = "JavaScript",
    JBIG2_DECODE = "JBIG2Decode",
    JBIG2_GLOBALS = "JBIG2Globals",
    JPX_DECODE = "JPXDecode",
    JS = "JS",
    K = "K",
    KEYWORDS = "Keywords",
    KEY_USAGE = "KeyUsage",
    KIDS = "Kids",
    L = "L",
    LAB = "Lab",
    LANG = "Lang",
    LAST = "Last",
    LAST_CHAR = "LastChar",
    LAST_MODIFIED = "LastModified",
    LC = "LC",
    LE = "LE",
    LEADING = "Leading",
    LEGAL_ATTESTATION = "LegalAttestation",
    LENGTH = "Length",
    LENGTH1 = "Length1",
    LENGTH2 = "Length2",
    LIGHTEN = "Lighten",
    LIMITS = "Limits",
    LJ = "LJ",
    LL = "LL",
    LLE = "LLE",
    LLO = "LLO",
    LOCATION = "Location",
    LUMINOSITY = "Luminosity",
    LW = "LW",
    LZW_DECODE = "LZWDecode",
    LZW_DECODE_ABBREVIATION = "LZW",
    M = "M",
    MAC = "Mac",
    MAC_EXPERT_ENCODING = "MacExpertEncoding",
    MAC_ROMAN_ENCODING = "MacRomanEncoding",
    MARK_INFO = "MarkInfo",
    MASK = "Mask",
    MATRIX = "Matrix",
    MAX_LEN = "MaxLen",
    MAX_WIDTH = "MaxWidth",
    MCID = "MCID",
    MDP = "MDP",
    MEDIA_BOX = "MediaBox",
    MEASURE = "Measure",
    METADATA = "Metadata",
    MISSING_WIDTH = "MissingWidth",
    MIX = "Mix",
    MK = "MK",
    ML = "ML",
    MM_TYPE1 = "MMType1",
    MOD_DATE = "ModDate",
    MULTIPLY = "Multiply",
    N = "N",
    NAME = "Name",
    NAMES = "Names",
    NEED_APPEARANCES = "NeedAppearances",
    NEW_WINDOW = "NewWindow",
    NEXT = "Next",
    NM = "NM",
    NON_EFONT_NO_WARN = "NonEFontNoWarn",
    NON_FULL_SCREEN_PAGE_MODE = "NonFullScreenPageMode",
    NONE = "None",
    NORMAL = "Normal",
    NUMS = "Nums",
    O = "O",
    OBJ = "Obj",
    OBJ_STM = "ObjStm",
    OC = "OC",
    OCG = "OCG",
    OCGS = "OCGs",
    OCPROPERTIES = "OCProperties",
    OE = "OE",
    OID = "OID",
    OFF = "OFF",
    Off = "Off",
    ON = "ON",
    OP = "OP",
    OP_NS = "op",
    OPEN_ACTION = "OpenAction",
    OPEN_TYPE = "OpenType",
    OPM = "OPM",
    OPT = "Opt",
    ORDER = "Order",
    ORDERING = "Ordering",
    OS = "OS",
    OUTLINES = "Outlines",
    OUTPUT_CONDITION = "OutputCondition",
    OUTPUT_CONDITION_IDENTIFIER = "OutputConditionIdentifier",
    OUTPUT_INTENT = "OutputIntent",
    OUTPUT_INTENTS = "OutputIntents",
    OVERLAY = "Overlay",
    P = "P",
    PAGE = "Page",
    PAGE_LABELS = "PageLabels",
    PAGE_LAYOUT = "PageLayout",
    PAGE_MODE = "PageMode",
    PAGES = "Pages",
    PAINT_TYPE = "PaintType",
    PANOSE = "Panose",
    PARAMS = "Params",
    PARENT = "Parent",
    PARENT_TREE = "ParentTree",
    PARENT_TREE_NEXT_KEY = "ParentTreeNextKey",
    PATH = "Path",
    PATTERN = "Pattern",
    PATTERN_TYPE = "PatternType",
    PDF_DOC_ENCODING = "PDFDocEncoding",
    PERMS = "Perms",
    PG = "Pg",
    PRE_RELEASE = "PreRelease",
    PREDICTOR = "Predictor",
    PREV = "Prev",
    PRINT_AREA = "PrintArea",
    PRINT_CLIP = "PrintClip",
    PRINT_SCALING = "PrintScaling",
    PROC_SET = "ProcSet",
    PROCESS = "Process",
    PRODUCER = "Producer",
    PROP_BUILD = "Prop_Build",
    PROPERTIES = "Properties",
    PS = "PS",
    PUB_SEC = "PubSec",
    Q = "Q",
    QUADPOINTS = "QuadPoints",
    R = "R",
    RANGE = "Range",
    RC = "RC",
    RD = "RD",
    REASON = "Reason",
    REASONS = "Reasons",
    REPEAT = "Repeat",
    RECIPIENTS = "Recipients",
    RECT = "Rect",
    REGISTRY = "Registry",
    REGISTRY_NAME = "RegistryName",
    RENAME = "Rename",
    RESOURCES = "Resources",
    RGB = "RGB",
    RI = "RI",
    ROLE_MAP = "RoleMap",
    ROOT = "Root",
    ROTATE = "Rotate",
    ROWS = "Rows",
    RUN_LENGTH_DECODE = "RunLengthDecode",
    RUN_LENGTH_DECODE_ABBREVIATION = "RL",
    RV = "RV",
    S = "S",
    SA = "SA",
    SATURATION = "Saturation",
    SCREEN = "Screen",
    SE = "SE",
    SEPARATION = "Separation",
    SET_F = "SetF",
    SET_FF = "SetFf",
    SHADING = "Shading",
    SHADING_TYPE = "ShadingType",
    SIG = "Sig",
    SIG_FLAGS = "SigFlags",
    SIZE = "Size",
    SM = "SM",
    SMASK = "SMask",
    SOFT_LIGHT = "SoftLight",
    SOUND = "Sound",
    SS = "SS",
    ST = "St",
    STANDARD_ENCODING = "StandardEncoding",
    STATE = "State",
    STATE_MODEL = "StateModel",
    STATUS = "Status",
    STD_CF = "StdCF",
    STEM_H = "StemH",
    STEM_V = "StemV",
    STM_F = "StmF",
    STR_F = "StrF",
    STRUCT_PARENT = "StructParent",
    STRUCT_PARENTS = "StructParents",
    STRUCT_TREE_ROOT = "StructTreeRoot",
    STYLE = "Style",
    SUB_FILTER = "SubFilter",
    SUBJ = "Subj",
    SUBJECT = "Subject",
    SUBJECT_DN = "SubjectDN",
    SUBTYPE = "Subtype",
    SUPPLEMENT = "Supplement",
    SV = "SV",
    SV_CERT = "SVCert",
    SW = "SW",
    SY = "Sy",
    SYNCHRONOUS = "Synchronous",
    T = "T",
    TARGET = "Target",
    TEMPLATES = "Templates",
    THREADS = "Threads",
    THUMB = "Thumb",
    TI = "TI",
    TILING_TYPE = "TilingType",
    TIME_STAMP = "TimeStamp",
    TITLE = "Title",
    TK = "TK",
    TM = "TM",
    TO_UNICODE = "ToUnicode",
    TR = "TR",
    TR2 = "TR2",
    TRAPPED = "Trapped",
    TRANS = "Trans",
    TRANSPARENCY = "Transparency",
    TREF = "TRef",
    TRIM_BOX = "TrimBox",
    TRUE_TYPE = "TrueType",
    TRUSTED_MODE = "TrustedMode",
    TU = "TU",
    TX = "Tx",
    TYPE = "Type",
    TYPE0 = "Type0",
    TYPE1 = "Type1",
    TYPE3 = "Type3",
    U = "U",
    UE = "UE",
    UF = "UF",
    UNCHANGED = "Unchanged",
    UNIX = "Unix",
    URI = "URI",
    URL = "URL",
    URL_TYPE = "URLType",
    V = "V",
    VERISIGN_PPKVS = "VeriSign.PPKVS",
    VERSION = "Version",
    VERTICES = "Vertices",
    VERTICES_PER_ROW = "VerticesPerRow",
    VIEW_AREA = "ViewArea",
    VIEW_CLIP = "ViewClip",
    VIEWER_PREFERENCES = "ViewerPreferences",
    VOLUME = "Volume",
    VP = "VP",
    W = "W",
    W2 = "W2",
    WHITE_POINT = "WhitePoint",
    WIDGET = "Widget",
    WIDTH = "Width",
    WIDTHS = "Widths",
    WIN_ANSI_ENCODING = "WinAnsiEncoding",
    XFA = "XFA",
    X_STEP = "XStep",
    XHEIGHT = "XHeight",
    XOBJECT = "XObject",
    XREF = "XRef",
    XREF_STM = "XRefStm",
    Y_STEP = "YStep",
    YES = "Yes"
}

export enum NPDFPaintOp {
    RectOp = "re",
    ConcatMatrixOp = "cm",
    CurveToOp = "c",
    EndPathNoFillOrStrokeOp = "n",
    FillOp = "f",
    FillEvenOddOp = "f*",
    InvokeXObjectOp = "Do",
    LineToOp = "l",
    BeginMarkedContentOp = "BMC",
    EndMarkedContentOp = "EMC",
    TextPosOp = "Td",
    MoveToOp = "m",
    CharSpacingOp = "Tc",
    CMYKOp = "k",
    CMYKStrokeOp = "K",
    DashOp = "d",
    GreyOp = "g",
    GreyStrokeOp = "G",
    LineCapOp = "J",
    LineJoinOp = "j",
    LineWidthOp = "w",
    NonZeroWindingClipOp = "W",
    RGBOp = "rg",
    RGBStrokeOp = "RG",
    FontAndSizeOp = "Tf",
    ShowTextOp = "Tj",
    RestoreOp = "Q",
    SaveOp = "q",
    StrokeOp = "S",
    BeginTextOp = "BT",
    EndTextOp = "ET"
}
