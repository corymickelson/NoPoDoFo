## NPDFFontMetrics
Follow link for typedef. Font Metrics interface.
```typescript
interface NPDFFontMetrics {
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
```

## NPDFcmyk
```type NPDFcmyk = [number, number, number, number]```

## NPDFrgb
```type NPDFrgb = [number, number, number]```

## NPDFGrayScal
```type NPDFGrayScale = number```

## NPDFColor
Values must be between 0.0 and 1.0 `type NPDFColor = NPDFrgb | NPDFcmyk | NPDFGrayScale`

## NPDFPoint
```type NPDFPoint = { x: number, y: number }```

## NPDFStokeStyle
Stroke style enum. Follow link for typedef.
```typescript
enum NPDFStokeStyle {
  Solid,
  Dash,
  Dot,
  DashDot,
  DashDotDot
}
```

## NPDFHighlightingMode
Highlight modes enum. Follow link for typedef.
```typescript
enum NPDFHighlightingMode {
  None,           
  Invert,         
  InvertOutline,  
  Push,           
  Unknown = 0xff
}
```


## NPDFFontType
Font types enum. Follow link for typedef.
``` typescript
enum NPDFFontType {
  TrueType,
  Type1Pfa,
  Type1Pfb,
  Type1Base14,
  Type3
}
```

## NPDFColorSpace
Color space enum. Follow link for typedef.
```typescript
enum NPDFColorSpace {
  DeviceGray,
  DeviceRGB,
  DeviceCMYK,
  Separation,
  CieLab,
  Indexed
}
```

## NPDFTextRenderingMode
Text rendering modes enum. Follow link for typedef.
```typescript
enum NPDFTextRenderingMode {
  Fill,
  Stroke,
  FillAndStroke,
  Invisible,
  FillToClipPath,
  StrokeToClipPath,
  FillAndStrokeToClipPath,
  ToClipPath
}
```

## NPDFLineCapStyle
Linecap styles enum. Follow link for typedef.
```typescript
enum NPDFLineCapStyle {
  Butt,
  Round,
  Square
}
```

## NPDFLineJoinStyle
Linejoin style enum. Follow link for typedef.
```typescript
enum NPDFLineJoinStyle {
  Miter,
  Round,
  Bevel
}
```

## NPDFVerticalAlignment
Vertical alignment enum. Follow link for typedef.

```typescript
/**
 * Top - Align with the top of the containing Rect
 * Center - Aligns Center, uses the set font's font metrics for calculating center
 * Bottom - Aligns with the bottom of the containing Rect
 */
enum NPDFVerticalAlignment {
  Top,
  Center,
  Bottom
}
```

## NPDFAlignment
Alignment enum. Follow link for typedef.
```typescript
/**
 * Left - Does nothing, this is the default behaviour
 * Center - Calculates center using font's font metrics stringWidth operation ( / 2)
 * Right - Calculates center using font's font metrics stringWidth operation
 */
enum NPDFAlignment {
  Left,
  Center,
  Bottom
}
```

## NPDFDictionaryKeyType
```type NPDFDictionaryKeyType = 'boolean' | 'long' | 'name' | 'real' ```

## NPDFInternal
NPDFInternal is used to represent an internal pointer or data structure. This data type is not exported as a Javascript value. ```type NPDFInternal = any ```

## NPDFCoerceKeyType
```type NPDFCoerceKeyType = 'boolean' | 'long' | 'name' | 'real' ```

## NPDFDataType
```type NPDFDataType = 'Boolean' | 'Number' | 'Name' | 'Real' | 'String' | 'Array' | 'Dictionary' | 'Reference' | 'RawData' ```

## SigFlags
Signature flags enum. Follow link for typedef.
```typescript
enum SigFlags {
  SignatureExists = 1,
  AppendOnly = 2,
  SignatureExistsAppendOnly = 3
}

```
## NPDFImageFormat
Supported image formats; enum. Follow link for typedef.
```typescript
enum NPDFImageFormat {
  data,
  png,
  tiff,
  jpeg,
}
```

## NPDFFontEncoding
Encodings enum. Follow link for typedef.
```typescript
enum NPDFFontEncoding {
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
```

## NPDFPageMode
Page mode enum. Follow link for typedef.
```typescript
enum NPDFPageMode {
  DontCare,
  UseNone,
  UseThumbs,
  UseBookmarks,
  FullScreen,
  UseOC,
  UseAttachments
}
```

## NPDFPageLayout
Page layout enum. Follow link for typedef.
```typescript
enum NPDFPageLayout {
  Ignore,
  Default,
  SinglePage,
  OneColumn,
  TwoColumnLeft,
  TwoColumnRight,
  TwoPageLeft,
  TwoPageRight
}
```

## NPDFCreateFontOpts
Font options interface. Follow link for typedef.
```typescript
interface NPDFCreateFontOpts {
  fontName: string,
  bold?: boolean,
  italic?: boolean,
  encoding?: NPDFFontEncoding,
  embed?: boolean,
  fileName?: string
}
```

## NPDFAnnotationAppearance
Annotation appearance enum. Follow link for typedef.
```typescript
enum NPDFAnnotationAppearance {
  normal,
  rollover,
  down
}
```

## ListItem
List field, list item. ```typescript type ListItem = { value: string, display: string } ```

## NPDFTextFieldOpts
Textfield options interface. Follow link for typedef.
```typescript
interface NPDFTextFieldOpts {
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
```

## NPDFCertificatePermission
Certificate permissions enum. Follow link for typedef.
```typescript
enum NPDFCertificatePermission {
  NoPerms = 1,
  FormFill = 2,
  Annotations = 3,
}
```

## NPDFFieldType
Field types enum. Follow link for typedef.
```typescript
enum NPDFFieldType {
  PushButton,
  CheckBox,
  RadioButton,
  TextField,
  ComboBox,
  ListBox,
  Signature,
  Unknown = 0xff
}
```

## NPDFAnnotationBorderStyle
```type NPDFAnnotationBorderStyle = { horizontal: number, vertical: number, width: number } ```

## ProtectionOption
```type ProtectionOption = 'Copy' | 'Print' | 'Edit' | 'EditNotes' | 'FillAndSign' | 'Accessible' | 'DocAssembly' | 'HighPrint' ```

## EncryptOption
Encryption constructor options. Follow link for typedef.
```typescript
type EncryptOption = {
  userPassword?: string
  ownerPassword?: string
  protection?: Array<ProtectionOption>
  algorithm?: 'rc4v1' | 'rc4v2' | 'aesv2' | 'aesv3'
  keyLength?: number
}
```
## ProtectionSummary
Encryption protections summary. Follow link for typedef
```typescript
type ProtectionSummary = {
  Accessible: boolean
  Print: boolean
  Copy: boolean
  DocAssembly: boolean
  Edit: boolean
  EditNotes: boolean
  FillAndSign: boolean
  HighPrint: boolean
}
```

## NPDFDestinationType
Destination type enum. Follow link for typedef.
```typescript
enum NPDFDestinationType {
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
```

## NPDFDestinationFit
Destination fit enum. Follow link for typedef.
```typescript
enum NPDFDestinationFit {
  Fit,
  FitH,
  FitV,
  FitB,
  FitBH,
  FitBV,
  Unknown = 0xFF
}
```

## NPDFAnnotation
Annotation enum. An annotation must be one of these types. Follow link for typedef.
```typescript
enum NPDFAnnotation {
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
  FileAttachement,
  Sound,          
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
```

## NPDFAnnotationFlag
Annotation flags enum. Follow link for typedef.
```typescript
enum NPDFAnnotationFlag {
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
```

## NPDFAnnotationType
Annotation type string enum. Follow link for typedef.
```typescript
enum NPDFAnnotationType {
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
```

## NPDFInfo
Document information interface. Follow link for typedef.
```typescript
interface NPDFInfo {
  author: string
  createdAt: Date
  creator: string
  keywords: string
  producer: string
  subject: string
  title: string
}
```

## NPDFVersion
PDF version enum. Follow link for typedef.
```typescript
enum NPDFVersion {
  Pdf11,
  Pdf12,
  Pdf13,
  Pdf14,
  Pdf15,
  Pdf16,
  Pdf17,
}
```

## NPDFWriteMode
Write modes enum. Follow link for typedef.
```typescript
enum NPDFWriteMode {
  Default = 0x01,
  Compact = 0x02
}
```

## NPDFActions
PDF Action types enum. Follow link for typedef.
```typescript
enum NPDFActions {
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
```

## NPDFMouseEvent
Mouse events enum. Follow link for typedef.
```typescript
enum NPDFMouseEvent {
  up,
  down,
  enter,
  exit
}
```

## NPDFPageEvent
Page events enum. Follow link for typedef.
```typescript
enum NPDFPageEvent {
  open,
  close,
  visible,
  invisible
}
```

## NPDFBlendMode
Blend modes string enum. Follow link for typedef.
```typescript
enum NPDFBlendMode {
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
```

## NPDFRenderingIntent
Rendering intent enum. Follow link for typedef.
```typescript
enum NPDFRenderingIntent {
  AbsoluteColorimetric = "AbsoluteColorimetric",
  RelativeColorimetric = "RelativeColorimetric",
  Perceptual = "Perceptual",
  Saturation = "Saturation"
}
```