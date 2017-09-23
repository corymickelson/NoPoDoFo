// import {Document as doc, IDocument as idoc, IPage as ipage} from './document'
// import {Annotation,IAnnotation, NpdfAction, NPdfAnnotation, NpdfAnnotationFlag} from './annotation'
// import {Field, FieldType, IField, IFieldInfo, ITextField, TextField} from './field'
// import {IImage, Image} from './image'
// import {IPainter, Painter} from './painter'
// import {IRect, Rect} from './rect'
// import {ISignature, Signature} from './signature'

import * as doc from './document'
import * as annot from './annotation'
import * as fields from './field'
import * as image from './image'
import * as painter from './painter'
import * as rect from './rect'
import * as sign from './signature'

export const Document = doc,
    Annotation = annot,
    Field = fields,
    Image = image,
    Painter = painter,
    Rect = rect,
    Signature = sign
