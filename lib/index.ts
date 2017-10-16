import {Document as doc} from './document'
import {Dictionary as dict} from './dictionary'
import {Obj as obj} from './object'
import {Page as page} from './page'
import {
    Annotation as annot,
    NpdfAction as action,
    NPdfAnnotation as annotenum,
    NpdfAnnotationFlag as annotflags,
    NPdfAnnotationType as annottype
} from './annotation'
import {CheckBox as checkbox, Field as field, TextField as textfield} from './field'
import {Image as image} from './image'
import {Painter as painter} from './painter'
import {Rect as rect} from './rect'
import {Signature as sign} from './signature'

export const Document = doc,
    Page = page,
    Dictionary = dict,
    Obj = obj,
    Annotation = annot,
    NPdfAction = action,
    NPdfAnnotation = annotenum,
    NPdfAnnotationFlag = annotflags,
    NPdfAnnotationType = annottype,
    Field = field,
    CheckBox = checkbox,
    TextField = textfield,
    Image = image,
    Painter = painter,
    Rect = rect,
    Signature = sign,
    CONVERSION = 0.0028346456693

