# API Documentation for Annotation

* [Properties](#properties)
  * [flags](#flags)
  * [title](#title)
  * [content](#content)
  * [destination](#destination)
  * [action](#action)
  * [open](#open)
  * [quadPoints](#quadPoints)
  * [color](#color)
  * [attachment](#attachment)

* [Methods](#methods)
  * [setBorderStyle](#setBorderStyle)
  * [hasAppearanceStream](#hasAppearanceStream)
  * [setAppearanceStream](#setAppearanceStream)
  * [getType](#getType)

## NoPoDoF Annotation
An annotation associates an object to a specific location on a [Page](./page).
Annotations are the core of all pdf widgets including AcroForm fields, sticky notes, links, etc...
In nopodofo an Annotation may only be created via [createAnnotation](./page.md#createannotation).
Annotations are a lower level PDF object, please familiarize yourself with the PDF [Spec](https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/PDF32000_2008.pdf) for usage.

```typescript
  class Annotation {
    flags: NPDFAnnotationFlag
    title: string
    content: string
    destination: Destination
    action: Action
    open: boolean
    quadPoints: number[]
    color: Color
    attachment: FileSpec

    setBorderStyle(v: NPDFAnnotationBorderStyle): void
    hasAppearanceStream(): boolean
    setAppearanceStream(xobj: XObject): void
    getType(): NPDFAnnotationType
  }
```

## Properties

### flags
This gets or sets the NPDFAnnotationFlag value of the annotations `F` property.

### title
This gets or sets the annotations title `T` property.

### content
This property gets or sets an annotations content `Contents` property. The content of an annotation is an optional
property responsible for displaying text for the annotation, or an alternate description. This can be useful for accessiblity
to users with disablities.

### destination
This gets or sets an [Destination](./destination.md) object for link type annotations, the [Destination](./destination.md) is displayed when
the annotation is activated.

### action
This gets or sets an [Action](./action.md) object in the annotation [Dictionary](./dictionary.md). The [Actions](./action.md) action will
be performed when the annotation is activated.

### open
This gets or sets the open property. An annotation may be in one of two states, open or closed. When closed the annotation will appear on the page as an icon,
box or stamp, when a closed annotation is clicked it becomes activated performing exhibiting it's associated object.

## Methods

### setBorderStyle
An annotation my optionally be surrounded with a styled border when displayed or printed. This method will set the `BS` dictionary with the
NPDFAnnotationBorderStyle value provied as the first parameter.

### hasAppearanceStream
If an appearance stream `AS` is present in the annotation [Dictionary](./dictionary.md) returns true, else false.

### setAppearanceStream
Set the appearance stream `AS` property of the annotation [Dictionary](./dictionary.md).

### getType
Get the type of annotation as an NPDFAnnotationType value.